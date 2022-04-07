#pragma once

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h> 


#define EXPECTED_ARGUMENTS 3
#define PROGRAM_NAME 0
#define MATRIX_SIZE_INDEX 1
#define PRIOIRTY_INDEX 2
#define INTEGER_BASE 10
#define ERROR -1

enum {
    SUCCESS = 0,
    BAD_ARGS = 1,
    BAD_MATRIX_SIZE = 2,
    BAD_PRIORITY = 3,
    BAD_WRITE = 4,
    BAD_READ = 5,
    BAD_OPEN = 6,
    BAD_CLOSE = 7
};

struct matrix_computation
{
   int matrix_size;
   int priority;
}matrix_computation;


void usage_message();