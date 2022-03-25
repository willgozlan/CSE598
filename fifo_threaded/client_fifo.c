/*
 * William Gozlan (william.gozlan@wustl.edu)
 * Client program
 * Adopted From: https://stackoverflow.com/questions/8611035/proper-fifo-client-server-connection
*/

#include "client_fifo.h"


int main(int argc, char *argv[])
{
   int client_to_server;
   char *client_to_server_fifo = "/tmp/client_to_server_fifo";

   int server_to_client;

   char server_to_client_fifo[BUF_SIZE];
   
   int requested_priority, requested_matrix_size;
   double computation_result; 


   if(argc != EXPECTED_ARGUMENTS)
   {
      usage_message(argv[PROGRAM_NAME]);
      return BAD_ARGS;
   }

   errno = 0;
   requested_priority = strtol(argv[MATRIX_SIZE_INDEX], NULL, INTEGER_BASE);
   if(errno != 0){
      printf("Bad command line agument. Expected integer\n");
      usage_message(argv[PROGRAM_NAME]);
      return BAD_MATRIX_SIZE;
   }

   requested_matrix_size = strtol(argv[PRIOIRTY_INDEX], NULL, INTEGER_BASE);
   if(errno != 0){
      printf("Bad command line agument. Expected integer\n");
      usage_message(argv[PROGRAM_NAME]);
      return BAD_PRIORITY;
   }

   // Get location of FIFO
   if(sprintf(server_to_client_fifo, "/tmp/server_to_client_fifo_%d\n", get_pid()) < 0)
   {
      perror("sprintf");
      return BAD_SPRINTF;
   }


   struct matrix_computation mc;
   mc.matrix_size = requested_priority;
   mc.priority = requested_matrix_size;



   /* write str to the FIFO */
   client_to_server = open(client_to_server_fifo, O_WRONLY);
   server_to_client = open(server_to_client_fifo, O_RDONLY);

   if(client_to_server == ERROR || server_to_client == ERROR)
   {
      perror("open");
      return BAD_OPEN;
   }

   if(write(client_to_server, &mc, sizeof(struct matrix_computation)) == ERROR)
   {
      perror("write");
      return BAD_WRITE;
   }

   if(read(server_to_client, &computation_result, sizeof(computation_result)) == ERROR)
   {
      perror("READ");
      return BAD_READ;
   }

   printf("Sever sent back result of %lf\n", computation_result); 


   if(close(client_to_server) == ERROR || close(server_to_client) == ERROR)
   {
      perror("close");
      return BAD_CLOSE;
   }
   

   return SUCCESS;
}





void usage_message(char* program_name)
{
   printf("Usage: %s <matrix_size> <priority>\n", program_name);
}
