#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "dense_mm.h"

#define ERROR -1

struct matrix_computation
{
   int matrix_size;
   int priority;
}matrix_computation;

enum {
    SUCCESS = 0,
    BAD_FIFO = 1,
    BAD_OPEN = 2,
    BAD_CLOSE = 3,
    BAD_UNLINK = 4,
    BAD_WRITE = 5,
    BAD_READ = 6
};


void shutdown(void);