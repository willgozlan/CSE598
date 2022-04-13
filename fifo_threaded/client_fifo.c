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
   char shm_location[BUF_SIZE];

   
   int requested_priority, requested_matrix_size;
   double computation_result; 

   int print_flag = FALSE;

   if(argc != MIN_EXPECTED_ARGUMENTS && argc != MAX_EXPECTED_ARGUMENTS)
   {
      usage_message(argv[PROGRAM_NAME]);
      return BAD_ARGS;
   }

   errno = 0;
   requested_matrix_size = strtol(argv[MATRIX_SIZE_INDEX], NULL, INTEGER_BASE);
   if(errno != 0){
      printf("Bad command line agument. Expected integer\n");
      usage_message(argv[PROGRAM_NAME]);
      return BAD_MATRIX_SIZE;
   }

   requested_priority = strtol(argv[PRIOIRTY_INDEX], NULL, INTEGER_BASE);
   if(errno != 0){
      printf("Bad command line agument. Expected integer\n");
      usage_message(argv[PROGRAM_NAME]);
      return BAD_PRIORITY;
   }

   if(argc == MAX_EXPECTED_ARGUMENTS)
   {
      print_flag = TRUE;
   }

   // Get location of FIFO
   if(sprintf(server_to_client_fifo, "/tmp/server_to_client_fifo_%d", getpid()) < 0)
   {
      perror("sprintf");
      return BAD_SPRINTF;
   }
   if(sprintf(shm_location, "/shm_%d", getpid()) < 0)
   {
      perror("sprintf");
      return BAD_SPRINTF;
   }

   printf("%s\n", server_to_client_fifo);
    
   if(mkfifo(server_to_client_fifo, 0666) == ERROR)
   {
      perror("mkfifo");
      return BAD_FIFO;
   }


   
   struct shared_mem_struct * shm_mapped;

   // SHARED MEMORY:
  
   int shm_fd = shm_open(shm_location , O_RDWR | O_CREAT, S_IRWXU);
   if(shm_fd < 0)
   {
      perror("shm_open");
      return BAD_SHM_OPEN;
   }
   if(ftruncate(shm_fd, sizeof(struct shared_mem_struct)) == ERROR)
   {
      perror("ftruncate");
      return BAD_TRUNCATE;
   }

   shm_mapped = (struct shared_mem_struct *) mmap(NULL, sizeof(struct shared_mem_struct), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
   
   if(shm_mapped == MAP_FAILED)
   {
      perror("mmap");
      return BAD_MMAP;
   }

   // shm_mapped->data = calloc(requested_matrix_size*requested_matrix_size, sizeof(int));

   // if(shm_mapped->data == NULL)
   // {
   //    perror("calloc");
   //    return BAD_ALLOC;
   // }

   // Setup Matrices and Initialize Coefficients
   double value = 0.0;
   for(int i = 0; i < requested_matrix_size; ++i)
   {
      for(int j = 0; j < requested_matrix_size; ++j)
      {
         shm_mapped->dataMatrixA[i][j] = value;
         ++value;
      }
   }      

   value = 0.0;
   for(int i = 0; i < requested_matrix_size; ++i)
   {
      for(int j = 0; j < requested_matrix_size; ++j)
      {
         shm_mapped->dataMatrixB[i][j] = value;
         ++value;
      }
   }      


   // Setup struct data to send to server
   struct matrix_computation mc;
   mc.matrix_size = requested_matrix_size; 
   mc.priority = requested_priority;

   strncpy(mc.shm_location, shm_location, sizeof(mc.shm_location));
   strncpy(mc.server_to_client_path, server_to_client_fifo, sizeof(mc.server_to_client_path));

   client_to_server = open(client_to_server_fifo, O_WRONLY);

   if(client_to_server == ERROR)
   {
      perror("open");
      return BAD_OPEN;
   }

   // Send request to server
   if(write(client_to_server, &mc, sizeof(struct matrix_computation)) == ERROR)
   {
      perror("write");
      return BAD_WRITE;
   }

   server_to_client = open(server_to_client_fifo, O_RDONLY);// | O_NONBLOCK );

   if(server_to_client == ERROR)
   {
      perror("open");
      return BAD_OPEN;
   }


   // Wait for server to finish, blocking on read() call
   if(read(server_to_client, &computation_result, sizeof(computation_result)) == ERROR)
   {
      perror("READ");
      return BAD_READ;
   }

   printf("Sever completed computing matrix.\n"); 

   if(print_flag)
   {
      for(int i = 0; i < requested_matrix_size; ++i)
      {
         for(int j = 0; j < requested_matrix_size; ++j)
         {
            printf("%.2lf ", shm_mapped->dataMatrixA[i][j]);
         }
         printf("\n");
      }      
   }

   if(close(client_to_server) == ERROR || close(server_to_client) == ERROR)
   {
      perror("close");
      return BAD_CLOSE;
   }
   if(unlink(server_to_client_fifo) == ERROR)
   {
      perror("unlink");
      return BAD_UNLINK;
   }
   
   return SUCCESS;
}





void usage_message(char* program_name)
{
   printf("Usage: %s <matrix_size> <priority> <print (optional, anything)>\n", program_name);
}
