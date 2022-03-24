#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "dense_mm.h"

#define ERROR -1

struct matrix_computation
{
   int matrix_size;
   int priority;
}matrix_computation;

// struct pthread_create_args
// {
//    int matrix_size;
//    int server_to_client_id;
// }pthread_create_args;

enum {
    SUCCESS = 0,
    BAD_FIFO = 1,
    BAD_OPEN = 2,
    BAD_CLOSE = 3,
    BAD_UNLINK = 4,
    BAD_WRITE = 5,
    BAD_READ = 6,
    BAD_THREAD = 7
};


void shutdown(void);