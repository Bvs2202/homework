#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define IP_SERV "127.0.0.1"
#define PORT_SERV 7777

#define SIZE_BUFF 256

int main()
{
    int sockfd;
    struct sockaddr_in serv_addr;
    ssize_t byte_in;
    char buff[SIZE_BUFF] = {0};
    char input_cmd[SIZE_BUFF] = {0};
    char *msg = "Отправь сколько время!";

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_SERV);
    inet_pton(AF_INET, IP_SERV, &serv_addr.sin_addr);

    connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    while (1)
    {
        printf("Нажмите любую клавишу, что бы узнать время(нажмите \"В\" для выхода): ");
        memset(input_cmd, 0, sizeof(input_cmd));
        fgets(input_cmd, sizeof(input_cmd), stdin);
        if (strncmp(input_cmd, "В", sizeof(input_cmd)) == 0)
        {
            break;
        }
        send(sockfd, msg, strlen(msg), 0);
        printf("Сообщение отправлено!\n");

        byte_in = recv(sockfd, buff, sizeof(buff) - 1, 0);
        if (byte_in <= 0)
        {
            printf("Сервер отключился!\n");
            break;
        }
        buff[byte_in] = '\0';

        printf("Время: %s\n", buff);
    }

    close(sockfd);
    printf("Завершение программы...Досвидание!\n\n");

    return 0;
}