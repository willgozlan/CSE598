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

// Uncomment this to enable printing from server side
// #define PRINT_MATRIX_SERVER

// Uncomment to enable Cache Optimal Matrix Multiply
// #define CACHE_OPTIMAL_MM

void *dense_mm(void *void_args)
{

	unsigned index, row, col; // loop indicies
	unsigned squared_size;
	double *C;
	double matrix_compute_result;

	struct pthread_create_args *args = void_args;
	int matrix_size = args->matrix_size;
	int server_to_client = args->server_to_client_id;

	struct sched_param sp;

	char *shm_loc;
	double *shm_mapped;
	int shm_fd, memory_matrix_size, memory_matrix_B_offset_from_A;

#ifdef CACHE_OPTIMAL_MM
	int temp;
#endif

	if (args->single_core)
	{
		cpu_set_t *cpu_set;
		cpu_set = malloc(sizeof(cpu_set_t));
		if (cpu_set == NULL)
		{
			perror("malloc");
			if (write(server_to_client, &matrix_compute_result, sizeof(matrix_compute_result)) == ERROR)
			{
				perror("write");
				return NULL;
			}
		}

		CPU_ZERO(cpu_set);

		CPU_SET(CORE_ZERO, cpu_set);

		if (sched_setaffinity(THIS_THREAD, sizeof(cpu_set), cpu_set) == ERROR)
		{
			perror("sched_setaffinity");
			if (write(server_to_client, &matrix_compute_result, sizeof(matrix_compute_result)) == ERROR)
			{
				perror("write");
				return NULL;
			}
		}
	}

	sp.sched_priority = args->requested_priority;
	if (sched_setscheduler(THIS_THREAD, args->scheduler_policy, &sp) == ERROR)
	{
		perror("sched_setscheduler");
		if (write(server_to_client, &matrix_compute_result, sizeof(matrix_compute_result)) == ERROR)
		{
			perror("write");
			return NULL;
		}
		return NULL;
	}

	// Find shared memory location and open it
	shm_loc = args->shm_location;

	shm_fd = shm_open(shm_loc, O_RDWR, S_IRWXU);

	if (shm_fd == ERROR)
	{
		perror("shm_open");
		return NULL;
	}

	squared_size = matrix_size * matrix_size;

	memory_matrix_size = TWO_MATRICES * squared_size * sizeof(double);
	memory_matrix_B_offset_from_A = squared_size;

	shm_mapped = (double *)mmap(NULL, memory_matrix_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (shm_mapped == MAP_FAILED)
	{
		perror("mmap");
		return NULL;
	}

	if (matrix_size > SQRT_UINT32_MAX)
	{
		printf("ERROR: Matrix size must be between zero and 65536!\n");
		matrix_compute_result = 0.0;
		if (write(server_to_client, &matrix_compute_result, sizeof(matrix_compute_result)) == ERROR)
		{
			perror("write");
			return NULL;
		}
		return NULL;
	}

	C = calloc(squared_size, sizeof(double));
	if (C == NULL)
	{
		perror("calloc");
		if (write(server_to_client, &matrix_compute_result, sizeof(matrix_compute_result)) == ERROR)
		{
			perror("write");
			return NULL;
		}
		return NULL;
	}

	printf("Multiplying matrices using PID %d...\n", getpid());
	printf("Thread ID is %ld.\n", syscall(__NR_gettid));

#ifdef CACHE_OPTIMAL_MM
	// Cache Optimal MM

	for (index = 0; index < matrix_size; index++)
	{
		for (row = 0; row < matrix_size; row++)
		{

			temp = shm_mapped[row * matrix_size + index];

			for (col = 0; col < matrix_size; col++)
			{
				C[row * matrix_size + col] += temp * shm_mapped[memory_matrix_B_offset_from_A + (index * matrix_size + col)];
			}
		}
	}

#else
	// Original, Cache naive MM
	for (row = 0; row < matrix_size; ++row)
	{
		for (col = 0; col < matrix_size; ++col)
		{
			for (index = 0; index < matrix_size; ++index)
			{
				C[row * matrix_size + col] += shm_mapped[row * matrix_size + index] * shm_mapped[memory_matrix_B_offset_from_A + (index * matrix_size + col)];
			}
		}
	}
#endif

#ifdef PRINT_MATRIX_SERVER
	for (int row = 0; row < matrix_size; ++row)
	{
		for (int col = 0; col < matrix_size; ++col)
		{
			printf("%.2lf ", C[row * matrix_size + col]);
		}
		printf("\n");
	}
#endif

	for (int row = 0; row < matrix_size; ++row)
	{
		for (int col = 0; col < matrix_size; ++col)
		{
			shm_mapped[row * matrix_size + col] = C[row * matrix_size + col];
		}
	}

	matrix_compute_result = C[0];

	printf("Multiplication done...Wrote back values to client shared memory!\n");

	// Send back result to client
	if (write(server_to_client, &matrix_compute_result, sizeof(matrix_compute_result)) == ERROR)
	{
		perror("write");
		return NULL;
	}

	return NULL;
}
