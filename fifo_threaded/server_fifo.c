/*
 * William Gozlan (william.gozlan@wustl.edu)
 * Server program
 * Adopted From: https://stackoverflow.com/questions/8611035/proper-fifo-client-server-connection
*/

#include "server_fifo.h"

atomic_int server_on;


int main(void)
{
   int client_to_server;
   char *client_to_server_fifo = "/tmp/client_to_server_fifo";

   int server_to_client;
   
   struct matrix_computation mc;
   int read_return_val;

   pthread_t thread_id_matrix, thread_id_fifo_open;

   // Setup SIGNAL Handler for SIGINT, to cleanly exit
   struct sigaction sa; 
   sa.sa_handler = shutdown;
   sigaction(SIGINT, &sa, NULL);

   server_on = 1;
   errno = 0;

   /* create the FIFO with syscall */
   if(mkfifo(client_to_server_fifo, 0666) == ERROR)
   {
      perror("mkfifo");
      return BAD_FIFO;
   }
  

   /* open, read, and display the message from the FIFO */
   client_to_server = open(client_to_server_fifo, O_RDONLY);

   if(errno == EINTR)
   {
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


      // Spawn a thread to go off and do the work 
      struct pthread_create_args pthread_args;
      pthread_args.matrix_size = mc.matrix_size;
      pthread_args.server_to_client_id = server_to_client;
      pthread_args.requested_priority = mc.priority;


      if(pthread_create(&thread_id_matrix, NULL, &dense_mm, &pthread_args))
      {
         perror("pthread_create");
         return BAD_THREAD;
      }

      printf("Worker thread launched!\n");

      /* clear out struct from any data */
      memset(&mc, 0, sizeof(mc));
   }

   printf("Shutting down server\n");

   // TODO: Join threads once while(NOT_DONE) isn't set. 

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



void shutdown(int signum)
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
