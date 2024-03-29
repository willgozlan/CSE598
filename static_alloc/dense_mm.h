#pragma once
#define _GNU_SOURCE
#include <stdio.h>  
#include <stdlib.h> 
#include <unistd.h>
#include <sys/resource.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h> 
#include <sched.h>

#include "return_values.h"


#define SQRT_UINT32_MAX 65536
#define THIS_THREAD 0
#define CORE_ZERO 0


struct pthread_create_args
{
   int matrix_size;
   int server_to_client_id;
   int requested_priority;
   char shm_location[BUF_SIZE];
   int single_core;
   int scheduler_policy;
};

void* dense_mm(void* void_args);


#define SHARED_MEM_SIZE 1000
#define SHARED_MEM "/share_mem"

struct shared_mem_struct{
    volatile double dataMatrixA[SHARED_MEM_SIZE][SHARED_MEM_SIZE];
    volatile double dataMatrixB[SHARED_MEM_SIZE][SHARED_MEM_SIZE];
};
