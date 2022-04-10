#pragma once

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h> 
#include <string.h> 
#include <sys/mman.h>

#include "return_values.h"

#define MAX_EXPECTED_ARGUMENTS 4
#define MIN_EXPECTED_ARGUMENTS 3
#define PROGRAM_NAME 0
#define MATRIX_SIZE_INDEX 1
#define PRIOIRTY_INDEX 2
#define INTEGER_BASE 10
#define FALSE 0
#define TRUE 1


struct matrix_computation
{
    int matrix_size;
    int priority;
    char server_to_client_path[BUF_SIZE];
    char shm_location[BUF_SIZE];
}matrix_computation;

void usage_message();


#define SHARED_MEM_SIZE 1000
#define SHARED_MEM "/share_mem"

struct shared_mem_struct{
    volatile double dataMatrixA[SHARED_MEM_SIZE][SHARED_MEM_SIZE];
    volatile double dataMatrixB[SHARED_MEM_SIZE][SHARED_MEM_SIZE];
};