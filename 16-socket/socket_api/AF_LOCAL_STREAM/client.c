#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/un.h>
#include <unistd.h>

#define SIZE_BUFF 256

#define SERVER_PATH "/tmp/my_sock"

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
	struct sockaddr_un serv_addr;
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

	sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		return 1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sun_family = AF_LOCAL;
	strncpy(serv_addr.sun_path, SERVER_PATH, sizeof(serv_addr.sun_path));
	

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_un)) == -1)
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