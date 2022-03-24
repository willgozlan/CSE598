/*
 * William Gozlan (william.gozlan@wustl.edu)
 * Server program
 * Adopted From: https://stackoverflow.com/questions/8611035/proper-fifo-client-server-connection
*/

#include "server_fifo.h"

int main(void)
{
   int client_to_server;
   char *client_to_server_fifo = "/tmp/client_to_server_fifo";

   int server_to_client;
   char *server_to_client_fifo = "/tmp/server_to_client_fifo";
   
   struct matrix_computation mc;
   int read_return_val;

   pthread_t thread_id;

   /* create the FIFO with syscall */
   if(mkfifo(client_to_server_fifo, 0666) == ERROR)
   {
      perror("mkfifo");
      return BAD_FIFO;
   }
   
   if(mkfifo(server_to_client_fifo, 0666) == ERROR)
   {
      perror("mkfifo");
      return BAD_FIFO;
   }

   /* open, read, and display the message from the FIFO */
   client_to_server = open(client_to_server_fifo, O_RDONLY);
   server_to_client = open(server_to_client_fifo, O_WRONLY);

   if(client_to_server == ERROR || server_to_client == ERROR)
   {
      perror("open");
      return BAD_OPEN;
   }


   while (1)
   {

      read_return_val = read(client_to_server, &mc, sizeof(struct matrix_computation));
      /*
      * Put open of FIFO in seperate thread to open(fifo, WRONLY), then spin with while(1)
      */
      if(read_return_val == 0)
      {
         continue;
      }
      if(read_return_val == ERROR)
      {
         perror("read");
         return BAD_READ;
      }

      // If both values are zero, protocol to shutdown, for now 
      if(mc.matrix_size == 0 && mc.priority == 0)
      {
         break;
      }

      printf("Recieved matrix size %d and priority %d\n", mc.matrix_size, mc.priority);
      
      // dense_mm(mc.matrix_size, server_to_client);

      struct pthread_create_args pthread_args;
      pthread_args.matrix_size = mc.matrix_size;
      pthread_args.server_to_client_id = server_to_client;
      pthread_args.requested_priority = mc.priority;

      if(pthread_create(&thread_id, NULL, &dense_mm, &pthread_args))
      {
         perror("pthread_create");
         return BAD_THREAD;
      }

      printf("Thread launched \n");


      // if(write(server_to_client, &matrix_compute_result, sizeof(matrix_compute_result)) == ERROR)
      // {
      //    perror("write");
      //    return BAD_WRITE;
      // }

      /* clear out struct from any data */
      memset(&mc, 0, sizeof(mc));
   }


   printf("Shutdown server\n");

   if(close(client_to_server) == ERROR || close(server_to_client) == ERROR)
   {
      perror("close");
      return BAD_CLOSE;
   }

   if(unlink(client_to_server_fifo) == ERROR || unlink(server_to_client_fifo) == ERROR)
   {
      perror("unlink");
      return BAD_UNLINK;
   }

   return SUCCESS;
}



void shutdown(void)
{
   // TODO: Signal Handler to shutdown clean
}