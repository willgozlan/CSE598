/******************************************************************************
* 
* dense_mm.c
* 
* This program implements a dense matrix multiply and can be used as a
* hypothetical workload. 
*
* Usage: This program takes a single input describing the size of the matrices
*        to multiply. For an input of size N, it computes A*B = C where each
*        of A, B, and C are matrices of size N*N. Matrices A and B are filled
*        with random values. 
*
* Written Sept 6, 2015 by David Ferry
* Adopted Feb. 2022 by William Gozlan
******************************************************************************/

#include "dense_mm.h"

const int num_expected_args = 2;
const unsigned sqrt_of_UINT32_MAX = 65536;


void* dense_mm(void* void_args)
{

	unsigned index, row, col; //loop indicies
	unsigned squared_size;
	double *A, *B, *C;
   	double matrix_compute_result ; 

	struct pthread_create_args* args = void_args;
	int matrix_size = args->matrix_size;
	int server_to_client = args->server_to_client_id;


    if (setpriority(PRIO_PGRP, 0, args->requested_priority) == -1)
    {
        perror("setpriority");
		matrix_compute_result = 0.0;
		if(write(server_to_client, &matrix_compute_result, sizeof(matrix_compute_result)) == -1)
		{
			perror("write");
			return NULL;
		}
		return NULL;
    }
	// Shared Memory
	  char* shm_loc = args->shm_location;
      // Shared Memory 
      struct shared_mem_struct * shm_mapped;
      printf("%s\n", shm_loc);
      int shm_fd = shm_open(shm_loc , O_RDWR, S_IRWXU);
      if(shm_fd == ERROR)
      {
         perror("shm_open");
         return NULL;
      }
      shm_mapped = (struct shared_mem_struct *) mmap(NULL, sizeof(struct shared_mem_struct), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
      if(shm_mapped == MAP_FAILED)
      {
         perror("mmap");
         return NULL;
      }

		printf("Matrix Size: %d\n", matrix_size);
      printf("Shared Memory Values:\n");
      for(int i = 0; i < matrix_size*matrix_size; ++i)
      {
         printf("%d\n", shm_mapped->data[i]);
      }











	


	if(matrix_size > sqrt_of_UINT32_MAX ){
		printf("ERROR: Matrix size must be between zero and 65536!\n");
		matrix_compute_result = 0.0;
		if(write(server_to_client, &matrix_compute_result, sizeof(matrix_compute_result)) == -1)
		{
			perror("write");
			return NULL;
		}
		return NULL;
	}

	squared_size = matrix_size * matrix_size;

	printf("Generating matrices...\n");

	A = (double*) malloc( sizeof(double) * squared_size );
	B = (double*) malloc( sizeof(double) * squared_size );
	C = (double*) malloc( sizeof(double) * squared_size );

	for( index = 0; index < squared_size; index++ ){
		A[index] = (double) rand();
		B[index] = (double) rand();
		C[index] = 0.0;
	}

	printf("Multiplying matrices...\n");

	for( row = 0; row < matrix_size; row++ ){
		for( col = 0; col < matrix_size; col++ ){
			for( index = 0; index < matrix_size; index++){
			C[row*matrix_size + col] += A[row*matrix_size + index] *B[index*matrix_size + col];
			}	
		}
	}

	printf("Multiplication done!\n");
	matrix_compute_result = C[0];

    printf("matrix_compute_result is %lf ... sending result back to client\n", matrix_compute_result);

	// Send back result to client
	if(write(server_to_client, &matrix_compute_result, sizeof(matrix_compute_result)) == -1)
	{
		perror("write");
		return NULL;
	}
	

	return NULL;

}
