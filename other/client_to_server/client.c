#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define MY_SOCK_PATH "/zpath"
#define LISTEN_BACKLOG 50
#define PORT 35000

#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

int main(int argc, char *argv[])
{
    int sfd, cfd;
    char *msg;
    struct sockaddr_in my_addr;
    // char buffer[1024] = {0};

    msg = (char *)malloc(sizeof(char));

    // srand(time(NULL));                // Initialization for random
    // sprintf(msg, "%d", rand() % 100); // set msg to be random number

    printf("Enter message to send:\n");
    scanf("%s", msg);
 

    /* For command line option to specify path
       MY_SOCK_PATH =(char *)malloc(100);
       MY_SOCK_PATH = "/path";
       strncpy(MY_SOCK_PATH, argv[1], sizeof(argv[1]));*/

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

    printf("Sending message %s\n", msg);

    if (send(sfd, msg, strlen(msg), 0) < 0)
    {
        handle_error("send");
    }
    printf("Message of %s Send!\n", msg);

    // read(sfd, buffer, 1024);
    // printf("Server sent back message: %s\n", buffer);

    close(cfd);
    close(sfd);
    return 0;
}