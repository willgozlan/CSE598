/*
 * William Gozlan (william.gozlan@wustl.edu)
 * Client program
 * Adopted From: https://stackoverflow.com/questions/8611035/proper-fifo-client-server-connection
 */

#include "client_fifo.h"

// Uncomment to print testing of shared memory matrix setup
// #define TEST_SHM_MATRICES

int main(int argc, char *argv[])
{
   int client_to_server;
   char *client_to_server_fifo = "/tmp/client_to_server_fifo";

   int server_to_client;

   char server_to_client_fifo[BUF_SIZE];
   char shm_location[BUF_SIZE];

   int requested_priority, requested_matrix_size, matrix_square_size;
   double computation_result;

   double *shm_mapped;
   int shm_fd, memory_matrix_size, memory_matrix_B_offset_from_A;

   double value;

   int print_flag = FALSE;

   if (argc != MIN_EXPECTED_ARGUMENTS && argc != MAX_EXPECTED_ARGUMENTS)
   {
      usage_message(argv[PROGRAM_NAME]);
      return BAD_ARGS;
   }

   errno = 0;
   requested_matrix_size = strtol(argv[MATRIX_SIZE_INDEX], NULL, INTEGER_BASE);
   if (errno != SUCCESS)
   {
      printf("Bad command line agument. Expected integer\n");
      usage_message(argv[PROGRAM_NAME]);
      return BAD_MATRIX_SIZE;
   }

   requested_priority = strtol(argv[PRIOIRTY_INDEX], NULL, INTEGER_BASE);
   if (errno != SUCCESS)
   {
      printf("Bad command line agument. Expected integer\n");
      usage_message(argv[PROGRAM_NAME]);
      return BAD_PRIORITY;
   }

   if (argc == MAX_EXPECTED_ARGUMENTS)
   {
      print_flag = TRUE;
   }

   // Get location of FIFO
   if (sprintf(server_to_client_fifo, "/tmp/server_to_client_fifo_%d", getpid()) < SUCCESS)
   {
      perror("sprintf");
      return BAD_SPRINTF;
   }
   if (sprintf(shm_location, "/shm_%d", getpid()) < SUCCESS)
   {
      perror("sprintf");
      return BAD_SPRINTF;
   }

   printf("%s\n", server_to_client_fifo);

   if (mkfifo(server_to_client_fifo, READ_WRITE_PERMISSIONS) == ERROR)
   {
      perror("mkfifo");
      return BAD_FIFO;
   }

   matrix_square_size = requested_matrix_size * requested_matrix_size;

   // SHARED MEMORY:
   shm_fd = shm_open(shm_location, O_RDWR | O_CREAT, S_IRWXU);
   if (shm_fd < SUCCESS)
   {
      perror("shm_open");
      return BAD_SHM_OPEN;
   }
   memory_matrix_size = TWO_MATRICES * matrix_square_size * sizeof(double);
   memory_matrix_B_offset_from_A = matrix_square_size;

   if (ftruncate(shm_fd, memory_matrix_size) == ERROR)
   {
      perror("ftruncate");
      return BAD_TRUNCATE;
   }

   shm_mapped = (double *)mmap(NULL, memory_matrix_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

   if (shm_mapped == MAP_FAILED)
   {
      perror("mmap");
      return BAD_MMAP;
   }

   // Setup Matrices and Initialize Coefficients
   value = 0.0;
   for (int row = 0; row < requested_matrix_size; ++row)
   {
      for (int col = 0; col < requested_matrix_size; ++col)
      {
         shm_mapped[row * requested_matrix_size + col] = value;
         ++value;
      }
   }

   value = 0.0;
   for (int row = 0; row < requested_matrix_size; ++row)
   {
      for (int col = 0; col < requested_matrix_size; ++col)
      {
         shm_mapped[memory_matrix_B_offset_from_A + (row * requested_matrix_size + col)] = value;
         ++value;
      }
   }

#ifdef TEST_SHM_MATRICES
   printf("Matrix A\n");

   for (int row = 0; row < requested_matrix_size; ++row)
   {
      for (int col = 0; col < requested_matrix_size; ++col)
      {
         printf("%.2lf ", shm_mapped[row * requested_matrix_size + col]);
      }
      printf("\n");
   }

   printf("Matrix B\n");

   for (int row = 0; row < requested_matrix_size; ++row)
   {
      for (int col = 0; col < requested_matrix_size; ++col)
      {
         printf("%.2lf ", shm_mapped[memory_matrix_B_offset_from_A + (row * requested_matrix_size + col)]);
      }
      printf("\n");
   }
#endif

   // Setup struct data to send to server
   struct matrix_computation mc;
   mc.matrix_size = requested_matrix_size;
   mc.priority = requested_priority;

   strncpy(mc.shm_location, shm_location, sizeof(mc.shm_location));
   strncpy(mc.server_to_client_path, server_to_client_fifo, sizeof(mc.server_to_client_path));

   client_to_server = open(client_to_server_fifo, O_WRONLY);

   if (client_to_server == ERROR)
   {
      perror("open");
      return BAD_OPEN;
   }

   // Send request to server
   if (write(client_to_server, &mc, sizeof(struct matrix_computation)) == ERROR)
   {
      perror("write");
      return BAD_WRITE;
   }

   server_to_client = open(server_to_client_fifo, O_RDONLY); // | O_NONBLOCK );

   if (server_to_client == ERROR)
   {
      perror("open");
      return BAD_OPEN;
   }

   // Wait for server to finish, blocking on read() call
   if (read(server_to_client, &computation_result, sizeof(computation_result)) == ERROR)
   {
      perror("READ");
      return BAD_READ;
   }

   printf("Sever completed computing matrix.\n");

   if (print_flag)
   {
      for (int row = 0; row < requested_matrix_size; ++row)
      {
         for (int col = 0; col < requested_matrix_size; ++col)
         {
            printf("%.2lf ", shm_mapped[row * requested_matrix_size + col]);
         }
         printf("\n");
      }
   }

   if (close(client_to_server) == ERROR || close(server_to_client) == ERROR)
   {
      perror("close");
      return BAD_CLOSE;
   }
   if (unlink(server_to_client_fifo) == ERROR)
   {
      perror("unlink");
      return BAD_UNLINK;
   }

   return SUCCESS;
}

void usage_message(char *program_name)
{
   printf("Usage: %s <matrix_size> <priority> [p (to enable printing)]>\n", program_name);
}
