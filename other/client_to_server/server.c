#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <time.h>

#define MY_SOCK_PATH "/zpath"
#define LISTEN_BACKLOG 50
#define HTTP_QUIT 418
#define PORT 35000
#define MAX_EVENT 3

#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

int main(int argc, char *argv[])
{
    int sfd, cfd, opt, epoll_fd, num_events, i;

    struct sockaddr_in my_addr;
    // struct epoll_event epollEvent;
    struct epoll_event epollEvents[MAX_EVENT];
    struct epoll_event ev;
    FILE* file;
    char buffer[100];

    /*Create Socket with socket() syscall*/
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        handle_error("socket");
    }

    opt = 1;

    // Forcefully attaching socket to the port 35000
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        handle_error("setsockopt");
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(PORT);

    /*Create communication channel with bind() syscall*/
    if (bind(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in)) == -1)
    {
        handle_error("bind");
    }

    /*Listen for incoming connections with the listen() sycall*/
    if (listen(sfd, LISTEN_BACKLOG) == -1)
    {
        handle_error("listen");
    }

    /* Now we can accept incoming connections one
     *               at a time using accept(2). */

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        handle_error("epoll_create1");
    }

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = STDIN_FILENO;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1)
    {
        handle_error("epoll_ctl");
    }

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = sfd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sfd, &ev) == -1)
    {
        handle_error("epoll_ctl");
    }

    while (1)
    {
        num_events = epoll_wait(epoll_fd, epollEvents, MAX_EVENT, -1);

        if (num_events == -1)
        {
            handle_error("epoll_wait");
        }

        for (i = 0; i < num_events; ++i)
        {

            if ((epollEvents[i].events & (EPOLLIN | EPOLLET)) && (epollEvents[i].data.fd == STDIN_FILENO))
            {
                printf("Standard In Data Ready!\n");
                break;
            }

            if ((epollEvents[i].events & (EPOLLIN | EPOLLET)) && epollEvents[i].data.fd == sfd)
            {
                // Otherwise, sfd was changed so can accept connection:
                cfd = accept(sfd, NULL, NULL);

                if (cfd == -1)
                {
                    handle_error("accept");
                }

                ev.events = EPOLLRDHUP;
                ev.data.fd = cfd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cfd, &(ev)) == -1)
                {
                    handle_error("epoll_ctl");
                }
                printf("Client Connection Established, so Data is Ready!\n");
                file = fdopen(cfd, "r");
                if (file < 0)
                {
                    handle_error("starting file");
                }

                while (fscanf(file, "%s", buffer) != EOF)
                {
                    printf("%s\n", buffer);
                }

                fclose(file);
                break;
            }

            if ((epollEvents[i].events & EPOLLRDHUP) && epollEvents[i].data.fd == cfd)
            {
                printf("Closing client.\n");
                close(cfd);
                break;
            }
        }
    }
}