#pragma once

#include <stdio.h>  
#include <stdlib.h> 
#include <unistd.h>
#include <sys/resource.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h> 

#define ERROR -1
#define BUF_SIZE 100

struct pthread_create_args
{
   int matrix_size;
   int server_to_client_id;
   int requested_priority;
   char shm_location[BUF_SIZE];
};

void* dense_mm(void* void_args);


#define SHARED_MEM_SIZE 1000
#define SHARED_MEM "/share_mem"

struct shared_mem_struct{
    volatile int write_guard;
    volatile int read_guard;
    volatile int delete_guard;
    volatile double dataMatrixA[SHARED_MEM_SIZE][SHARED_MEM_SIZE];
    volatile double dataMatrixB[SHARED_MEM_SIZE][SHARED_MEM_SIZE];
};

enum {
    SUCCESS = 0,
    BAD_FIFO = 1,
    BAD_OPEN = 2,
    BAD_CLOSE = 3,
    BAD_UNLINK = 4,
    BAD_WRITE = 5,
    BAD_READ = 6,
    BAD_THREAD = 7,
    BAD_ALLOC = 8,
    BAD_SIGNAL = 9,
    BAD_SIGACTION = 10,
    BAD_MMAP = 11,
    BAD_SHM_OPEN = 12,
};