#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    char buffer[255];
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

    while (1)
    {
        memset(buffer, 0, 255);
        fgets(buffer, 255, stdin);
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0)
            error("Error on writing.");

        memset(buffer, 0, 255);
        n = read(sockfd, buffer, 255);
        if (n < 0)
            error("Error on reading.");

        printf("Server said: %s\n", buffer);

        if (!strncmp("bye", buffer, 3))
            break;
    }
    close(sockfd);
    return 0;
}