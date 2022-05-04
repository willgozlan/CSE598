/*
 * William Gozlan (william.gozlan@wustl.edu)
 * Server program
 * Adopted From: https://stackoverflow.com/questions/8611035/proper-fifo-client-server-connection
 */

#include "server_fifo.h"

atomic_int server_on = 1;

int main(int argc, char **argv)
{
   int client_to_server;
   char *client_to_server_fifo = "/tmp/client_to_server_fifo";

   int server_to_client;

   struct matrix_computation mc;
   int read_return_val;

   pthread_t thread_id_fifo_open;
   struct sched_param sp;

   int single_core = 0;
   int scheduler_policy = SCHED_RR;

   // Setup SIGNAL Handler for SIGINT, to cleanly exit
   struct sigaction sa;
   memset(&sa, 0, sizeof(sa));
   sa.sa_sigaction = shutdown;
   if (sigaction(SIGINT, &sa, NULL) < 0)
   {
      perror("sigaction");
      return BAD_SIGACTION;
   }

   // Check command options
   if (parse_command_line_args(argc, argv, &single_core, &scheduler_policy) == ERROR)
   {
      return usage_message(argv[PROGRAM_NAME]);
   }

   sp.sched_priority = HIGH_SCHED_PRIO;

   if (sched_setscheduler(0, scheduler_policy, &sp) == ERROR)
   {
      perror("sched_setscheduler");
      return BAD_SET_SCHED;
   }

   errno = 0;

   // Initialize linked list of clients
   List *list = make_list();

   /* create the FIFO with syscall */
   if (mkfifo(client_to_server_fifo, 0666) == ERROR)
   {
      perror("mkfifo");
      return BAD_FIFO;
   }

   client_to_server = open(client_to_server_fifo, O_RDONLY);

   if (errno == EINTR)
   {
      printf("Shutting down server\n");
      if (unlink(client_to_server_fifo) == ERROR)
      {
         perror("unlink");
         return BAD_UNLINK;
      }
      return SUCCESS;
   }

   if (client_to_server == ERROR)
   {
      perror("open");
      return BAD_OPEN;
   }

   // Hold other end of fifo open in a new thread
   if (pthread_create(&thread_id_fifo_open, NULL, &hold_fifo_open, client_to_server_fifo))
   {
      perror("pthread_create");
      return BAD_THREAD;
   }

   list_add(thread_id_fifo_open, list);

   while (server_on)
   {

      errno = 0;
      read_return_val = read(client_to_server, &mc, sizeof(struct matrix_computation));

      if (errno == EINTR)
      {
         break;
      }
      if (read_return_val == ERROR)
      {
         perror("read");
         return BAD_READ;
      }

      printf("Recieved matrix size %d and priority %d\n", mc.matrix_size, mc.priority);

      errno = 0;

      server_to_client = open(mc.server_to_client_path, O_WRONLY);
      if (errno == EINTR)
      {
         break;
      }
      if (server_to_client == ERROR)
      {
         perror("open");
         printf("Cannot open client... skipping\n");
         continue;
      }

      // Spawn a thread to go off and do the work, setting data in struct
      struct pthread_create_args pthread_args;
      pthread_args.matrix_size = mc.matrix_size;
      pthread_args.server_to_client_id = server_to_client;
      pthread_args.requested_priority = mc.priority;
      strncpy(pthread_args.shm_location, mc.shm_location, sizeof(pthread_args.shm_location));
      pthread_args.single_core = single_core;
      pthread_args.scheduler_policy = scheduler_policy;

      pthread_t thread_id_matrix;

      if (pthread_create(&thread_id_matrix, NULL, &dense_mm, &pthread_args))
      {
         perror("pthread_create");
         return BAD_THREAD;
      }

      list_add(thread_id_matrix, list);

      printf("Worker thread launched!\n");

      memset(&mc, 0, sizeof(mc));
   }

   printf("Shutting down server\n");

   // Stop all threads by looping through list of them
   Node *current = list->head;
   while (current != NULL)
   {
      if (pthread_cancel(current->data) == ERROR)
      {
         perror("pthread_cancel");
      }
      current = current->next;
   }

   list_destroy(list);

   if (close(client_to_server) == ERROR || close(server_to_client) == ERROR)
   {
      perror("close");
      return BAD_CLOSE;
   }

   if (unlink(client_to_server_fifo) == ERROR)
   {
      perror("unlink");
      return BAD_UNLINK;
   }

   return SUCCESS;
}

void shutdown(int signum, siginfo_t *siginfo, void *context)
{
   printf("SHUTDOWN signal received\n");
   server_on = 0;
}

void *hold_fifo_open(void *client_to_server_fifo)
{
   // Open the fifo, then spin until termination, to hold FIFO open
   if (open(client_to_server_fifo, O_WRONLY) == ERROR)
   {
      return NULL;
   }

   while (1)
      ;

   return NULL;
}

int parse_command_line_args(int argc, char **argv, int *singleCore, int *scheduler_policy)
{

   int opt;
   while ((opt = getopt(argc, argv, "cfrh")) != ERROR)
   {
      switch (opt)
      {
      case 'c':
         printf("Using single core.\n");
         *singleCore = 1;
         break;
      case 'f':
         printf("Using SCHED_FIFO scheduling policy.\n");
         *scheduler_policy = SCHED_FIFO;
         break;
      case 'r':
         printf("Using SCHED_RR scheduling policy.\n");
         *scheduler_policy = SCHED_RR;
         break;
      case 'h':
         printf("HELP:\n");
         return ERROR;
      case '?':
         printf("Unknown option: %c\n", optopt);
         return ERROR;
      }
   }

   return SUCCESS;
}

int usage_message(char *program_name)
{
   printf("Usage: %s [-c] [-f] [-r] [-h]\n", program_name);
   printf("-c flag means run with single core.\n");
   printf("-f flag means run using SCHED_FIFO scheduling policy.\n");
   printf("-r flag means run using SCHED_RR scheduling policy (default).\n");
   printf("-h to show this usage message for help.\n");
   return BAD_ARGS;
}