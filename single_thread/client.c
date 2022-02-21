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
    int sfd, cfd, requested_priority;
    struct sockaddr_in my_addr;
    char requested_priority_string[2];
    char buffer[5];
    // FILE* file;

    

    if(argc != 2)
    {
        printf("Usage: %s <requested_priority_value>\n", argv[0]);
        return -1;
    }

    errno = 0;
    requested_priority = strtol(argv[1], NULL, 10);
    if(errno != 0){
        handle_error("strtol");
    }
    sprintf(requested_priority_string, "%d", requested_priority);


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

    // printf("Sending message %s\n", msg);

    printf("Requesting Priority %s from server\n", requested_priority_string);

    if (send(sfd, requested_priority_string, strlen(requested_priority_string), 0) < 0)
    {
        handle_error("send");
    }

    // file = fdopen(sfd, "r");
    // if (file < 0)
    // {
    //     handle_error("starting file");
    // }

    // while (fscanf(file, "%s", buffer) != EOF)
    // {
    //     printf("%s\n", buffer);
    // }


    // fclose(file);
    // if(read(sfd, buffer, 2) < 0)
    // {
    //     handle_error("read");
    // }


    if(recv(sfd, buffer, 5, 0) < 0)
    {
        handle_error("recv");
    }

 


    printf("Server sent back message: %s\n", buffer);
    // errno = 0;
    // int recived_priority = strtol("87 buffer", NULL, 10);
    // if(errno != 0){
    //     handle_error("strtol");
    // }
    // printf("Server sent back message: %d\n", recived_priority);


    close(cfd);
    close(sfd);
    return 0;
}