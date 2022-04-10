#pragma once

#define ERROR -1
#define BUF_SIZE 100

enum {
    SUCCESS,
    BAD_FIFO,
    BAD_OPEN,
    BAD_CLOSE,
    BAD_UNLINK,
    BAD_WRITE,
    BAD_READ,
    BAD_THREAD, 
    BAD_ALLOC,
    BAD_SIGNAL, 
    BAD_SIGACTION, 
    BAD_MMAP,
    BAD_SHM_OPEN,
    BAD_ARGS,  
    BAD_MATRIX_SIZE,
    BAD_PRIORITY, 
    BAD_SPRINTF, 
    BAD_TRUNCATE,
    BAD_SET_SCHED
};