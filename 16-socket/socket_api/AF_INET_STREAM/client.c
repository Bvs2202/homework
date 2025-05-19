#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SIZE_BUFF 256

#define PORT 8080
#define SERVER_IP "127.0.0.1"

int sockfd = -1;

void clean(int signum)
{
	if (sockfd != -1)
	{
		close(sockfd);
	}
	printf("\n");

	exit(0);
}

int main()
{
	struct sockaddr_in serv_addr;
	char *msg = "Hello world!";
	char buffer[SIZE_BUFF] = {0};
	ssize_t byte_in;
	struct sigaction sa;
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sa.sa_handler = clean;
	sa.sa_mask = set;
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) < 0)
	{
		perror("sigaction");
		return 1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		return 1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)
	{
		perror("inet_pton");
		clean(1);
	}

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
	{
		perror("connect");
		clean(1);
	}

	if (send(sockfd, msg, strlen(msg), 0) == -1)
	{
		perror("send");
		clean(1);
	}
	printf("Отправлено сообщение: %s\n", msg);

	byte_in = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
	if (byte_in == -1)
	{
		perror("recv");
		clean(1);
	}
	buffer[byte_in] = '\0';

	printf("Пришло сообщение от сервера: %s\n", buffer);

	close(sockfd);

	return 0;
}