#pragma once

#include <stdio.h>  
#include <stdlib.h> 
#include <unistd.h>
#include <sys/resource.h>

struct pthread_create_args
{
   int matrix_size;
   int server_to_client_id;
   int requested_priority;
}pthread_create_args;

void* dense_mm(void* void_args);

