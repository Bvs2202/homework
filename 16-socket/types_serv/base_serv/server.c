#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define PORT_SERV 7777

#define SIZE_BUFF 256

void *send_time(void *arg)
{
    int new_sockfd = *(int *)arg;
    char buff[SIZE_BUFF] = {0};
    time_t cur_time;
    struct tm *time_info;
    ssize_t byte_in;

    while(1)
    {
        recv(new_sockfd, buff, sizeof(buff), 0);

        time(&cur_time);
        time_info = localtime(&cur_time);
        strftime(buff, sizeof(buff), "%H:%M:%S", time_info);

        send(new_sockfd, buff, strlen(buff), 0);
    }
}

int main()
{
    int sockfd;
    int new_sockfd;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t len_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_SERV);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    listen(sockfd, 5);

    len_addr = sizeof(client_addr);

    while (1)
    {
        pthread_t thread;

        new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &len_addr);

        pthread_create(&thread, NULL, send_time, (void *)&new_sockfd);
    }

    close(sockfd);

    return 0;
}