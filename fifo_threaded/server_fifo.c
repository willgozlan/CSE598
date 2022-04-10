/*
 * William Gozlan (william.gozlan@wustl.edu)
 * Server program
 * Adopted From: https://stackoverflow.com/questions/8611035/proper-fifo-client-server-connection
*/

 
/*      
   
Notes (4/7):        
   - Dynamic allocation of matrices isn't totally necissary, but cool. Static alocation ok. 
               Good how it is now.
               Make both versions and compare the 2. 
   - Trace on kernelshark 
   - Play with both sched FIFO and both schedRR
                  first one runs to completion     2 processes switched out
         Use sched_setscheduler in server, to set it since threads will inherent it.
   - Linux CPU Cgroups to assign budgets for each client process
   - Don't need locks for shared memory, as thought

            Notes(3/31): Mark Bober (EIT) (Or Support@EIT) To run on LinuxLab
            Change signal handler to Real Time Signal
            Shared Memory for Matrix Coefficents (POSIX Shared Memory)
   In Future: Permissions of the shared memory so no one has capability to access, until given
   Marion: CH. 54 LPI (Scoped Shared Memory) --> Step Farther (CH.48 LPI) System 5 shared memory 
                                                         Access Control Lists -> Closer to Capability Model
*/


#include "server_fifo.h"

atomic_int server_on = 1;


int main(void)
{
   int client_to_server;
   char *client_to_server_fifo = "/tmp/client_to_server_fifo";

   int server_to_client;
   
   struct matrix_computation mc;
   int read_return_val;

   pthread_t thread_id_fifo_open;
   struct sched_param sp;

   // Setup SIGNAL Handler for SIGINT, to cleanly exit
   struct sigaction sa; 
   memset (&sa, 0, sizeof(sa)); 
   sa.sa_sigaction = shutdown;
   if(sigaction(SIGINT, &sa, NULL) < 0)
   {
      perror("sigaction");
      return BAD_SIGACTION;
   }

   sp.sched_priority = HIGH_SCHED_PRIO;

   if(sched_setscheduler(0, SCHED_FIFO, &sp) == ERROR){  // SCHED_FIFO or SCHED_RR
      perror("sched_setscheduler");
      return BAD_SET_SCHED;
   }

   errno = 0;

   // Initialize linked list of clients
   List * list = make_list();

   /* create the FIFO with syscall */
   if(mkfifo(client_to_server_fifo, 0666) == ERROR)
   {
      perror("mkfifo");
      return BAD_FIFO;
   }
  

   client_to_server = open(client_to_server_fifo, O_RDONLY);

   if(errno == EINTR)
   {
      printf("Shutting down server\n");
      if(unlink(client_to_server_fifo) == ERROR) 
      {
         perror("unlink");
         return BAD_UNLINK;
      }
      return SUCCESS; 
   }

   if(client_to_server == ERROR)
   {
      perror("open");
      return BAD_OPEN;
   }
  
   // Hold other end of fifo open in a new thread
   if(pthread_create(&thread_id_fifo_open, NULL, &hold_fifo_open, client_to_server_fifo))
   {
         perror("pthread_create");
         return BAD_THREAD;
   }

   list_add(thread_id_fifo_open, list);

   while(server_on)
   {

      errno = 0;
      read_return_val = read(client_to_server, &mc, sizeof(struct matrix_computation));

      if(errno == EINTR)
      {
         break;
      }
      if(read_return_val == ERROR)
      {
         perror("read");
         return BAD_READ;
      }

      printf("Recieved matrix size %d and priority %d\n", mc.matrix_size, mc.priority);
      


      errno = 0;

      server_to_client = open(mc.server_to_client_path, O_WRONLY);
      if(errno == EINTR)
      {
         break;
      }
      if(server_to_client == ERROR)
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


      pthread_t thread_id_matrix;

      if(pthread_create(&thread_id_matrix, NULL, &dense_mm, &pthread_args))
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
   Node * current = list->head;
   while(current != NULL)
   {
      if(pthread_cancel(current->data) == ERROR){
         perror("pthread_cancel");
      }
      current = current->next;
   }
  
   list_destroy(list);

   if(close(client_to_server) == ERROR || close(server_to_client) == ERROR)
   {
      perror("close");
      return BAD_CLOSE;
   }

   if(unlink(client_to_server_fifo) == ERROR)
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

void* hold_fifo_open(void* client_to_server_fifo)
{
   // Open the fifo, then spin until termination, to hold FIFO open
   if(open(client_to_server_fifo, O_WRONLY) == ERROR)
   {
      return NULL;
   }

   while(1);

   return NULL;
}
