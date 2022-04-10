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

#define HIGH_SCHED_PRIO 99

struct matrix_computation
{
   int matrix_size;
   int priority;
   char server_to_client_path[BUF_SIZE];
   char shm_location[BUF_SIZE];
}matrix_computation;

void shutdown(int signum, siginfo_t *siginfo, void *context);

void* hold_fifo_open(void* client_to_server_fifo);
