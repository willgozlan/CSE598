#pragma once
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/sched.h>
#include <sys/types.h>
#include <sys/syscall.h>

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

void *dense_mm(void *void_args);

#define TWO_MATRICES 2
#define SHARED_MEM "/share_mem"
