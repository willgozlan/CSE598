#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <errno.h>
#include <sys/mman.h>


#include "list.h"
#include "dense_mm.h"

#define ERROR -1
#define BUF_SIZE 100

struct matrix_computation
{
   int matrix_size;
   int priority;
   char server_to_client_path[BUF_SIZE];
   char shm_location[BUF_SIZE];
}matrix_computation;


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


void shutdown(int signum, siginfo_t *siginfo, void *context);

void* hold_fifo_open(void* client_to_server_fifo);


// SHARED MEMORY

#define SHARED_MEM_SIZE 10
#define SHARED_MEM "/share_mem"

struct shared_mem_struct{
    volatile int write_guard;
    volatile int read_guard;
    volatile int delete_guard;
    volatile int data[SHARED_MEM_SIZE];
};
