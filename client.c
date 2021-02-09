#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

void *send_pro(void *arg);
void *recv_pro(void *arg);
int handle(void *arg);

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    //char buffer[255];

    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port.\n", argv[0]);
        exit(1);
    }

    portno = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("Error opening Socket.");

    memset((char *)&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(portno);

    printf("connecting Server!!!\n");

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Connection Failed.");

    printf("Server connected, you can say 'hello'\n");

    handle(&sockfd);

    close(sockfd);
    return 0;
}

int handle(void *arg)
{
    pthread_t recv_thr, send_thr;

    pthread_create(&recv_thr, NULL, recv_pro, arg);
    pthread_create(&send_thr, NULL, send_pro, arg);
    pthread_join(recv_thr, 0);
    pthread_join(send_thr, 0);
}

void *recv_pro(void *arg)
{
    char recv_buf[255];
    int n;
    int *sockfd = (int *)arg;
    while (1)
    {
        memset(recv_buf, 0, 255);
        n = read(*sockfd, recv_buf, 255);
        if (n < 0)
            error("Error on reading.");
        printf("Server said: %s", recv_buf);
        if (!strncmp("bye", recv_buf, 3))
            break;
    }
    return 0;
}
void *send_pro(void *arg)
{
    char send_buf[255];
    int n;
    int *sockfd = (int *)arg;
    while (1)
    {
        memset(send_buf, 0, 255);
        fgets(send_buf, 255, stdin);
        n = write(*sockfd, send_buf, strlen(send_buf));
        if (n < 0)
            error("Error on writing.");
        if (!strncmp("bye", send_buf, 3))
            break;
    }
    return 0;
}