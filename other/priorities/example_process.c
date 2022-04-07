#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/resource.h>


int getThisProcessPriority()
{
    int ret;
    errno = 0;
    ret = getpriority(PRIO_PROCESS, 0);
    if(errno != 0)
    {
        printf("Call to get getpriority failed\n");
        perror("getpriority");
    }
    return ret;
}

void changePriority(int newPriority)
{
    int ret = setpriority (PRIO_PGRP, 0, newPriority);
    if (ret == -1)
    {
        perror ("setpriority");
    }
}

long double hardComputation(int rounds)
{
    int i,j;
    long double accumulator = 1.1;

    for(j = 0; j < rounds; ++j){
        for(i = 0; i < rounds; ++i)
        {
            accumulator *= accumulator; 
        }
    }

    return accumulator; 
}

int main(int argc, char *argv[])
{
    printf("Priority is %d\n", getThisProcessPriority());
   
    changePriority(20);

    printf("Priority is %d\n", getThisProcessPriority());

    printf("Computation Result is %Lf\n", hardComputation(1000));


    return 0;
}