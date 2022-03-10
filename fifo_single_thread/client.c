#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/resource.h>
#include "dense_mm.h"

#define MY_SOCK_PATH "/zpath"
#define LISTEN_BACKLOG 50
#define PORT 35001

#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

int main(int argc, char *argv[])
{
    int sfd, cfd, given_priority; // requested_priority, given_priority;
    struct sockaddr_in my_addr;
    // char requested_priority_string[2];
    char buffer[5];
    // FILE* file;

    

    if(argc != 2)
    {
        // printf("Usage: %s <requested_priority_value>\n", argv[0]);
        printf("Usage: %s <workload_amount>\n", argv[0]);
        return -1;
    }
/*  TODO: Use to request a priority
    errno = 0;
    requested_priority = strtol(argv[1], NULL, 10);
    if(errno != 0){
        handle_error("strtol");
    }
    sprintf(requested_priority_string, "%d", requested_priority); */


    /*Create Socket with socket() syscall*/
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        handle_error("socket");
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    inet_aton("localhost", &(my_addr.sin_addr));

    /*Connect to server connect() syscall*/
    cfd = connect(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in));
    if (cfd < 0)
    {
        handle_error("connect");
    }

    /* TODO: Used to request a prioirty
    printf("Requesting Priority %s from server\n", requested_priority_string);

    if (send(sfd, requested_priority_string, strlen(requested_priority_string), 0) < 0)
    {
        handle_error("send");
    }*/


    if(recv(sfd, buffer, 5, 0) < 0)
    {
        handle_error("recv");
    }

    close(cfd);
    close(sfd);

 
    errno = 0;
    given_priority = strtol(buffer, NULL, 10);
    if(errno != 0){
        handle_error("strtol");
    }

    printf("Priority allowed is: %d\n", given_priority);

    if (setpriority (PRIO_PGRP, 0, given_priority) == -1)
    {
        perror ("setpriority");
    }

    printf("Priority set to: %d\n", getpriority(PRIO_PROCESS, 0));

    dense_mm(argv[1]);



    return 0;
}