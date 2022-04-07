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


#define EXPECTED_ARGUMENTS 3
#define PROGRAM_NAME 0
#define MATRIX_SIZE_INDEX 1
#define PRIOIRTY_INDEX 2
#define INTEGER_BASE 10
#define ERROR -1
#define BUF_SIZE 100

enum {
    SUCCESS = 0,
    BAD_ARGS = 1,
    BAD_MATRIX_SIZE = 2,
    BAD_PRIORITY = 3,
    BAD_WRITE = 4,
    BAD_READ = 5,
    BAD_OPEN = 6,
    BAD_CLOSE = 7,
    BAD_SPRINTF = 8,
    BAD_FIFO = 9,
    BAD_UNLINK = 10,
    BAD_MMAP = 11,
    BAD_TRUNCATE = 12,
    BAD_SHM_OPEN = 13,
    BAD_ALLOC = 14
};

struct matrix_computation
{
    int matrix_size;
    int priority;
    char server_to_client_path[BUF_SIZE];
    char shm_location[BUF_SIZE];
}matrix_computation;

void usage_message();


// SHARED MEMORY

#define SHARED_MEM_SIZE 1000
#define SHARED_MEM "/share_mem"

struct shared_mem_struct{
    volatile int write_guard;
    volatile int read_guard;
    volatile int delete_guard;
    volatile double dataMatrixA[SHARED_MEM_SIZE][SHARED_MEM_SIZE];
    volatile double dataMatrixB[SHARED_MEM_SIZE][SHARED_MEM_SIZE];
};