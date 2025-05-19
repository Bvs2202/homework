#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/un.h>
#include <unistd.h>

#define SIZE_BUFF 256

#define PORT 8080
#define SIZE_LISTEN 5
#define SERVER_PATH "/tmp/my_sock"

int sockfd = -1;
int new_sockfd = -1;

void clean(int signum)
{
	unlink(SERVER_PATH);
	if (new_sockfd != -1)
	{
		close(new_sockfd);
	}
	if (sockfd != -1)
	{
		close(sockfd);
	}
	printf("\n");

	exit(1);
}

int main()
{
	struct sockaddr_un serv_addr, client_addr;
	char buffer[SIZE_BUFF] = {0};
	ssize_t byte_in;
	sigset_t set;
	struct sigaction sa;
	socklen_t len_client_addr;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sa.sa_handler = clean;
	sa.sa_mask = set;
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) < 0)
	{
		perror("sigaction");
		exit(1);
	}

	sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		exit(1);
	}

	unlink(SERVER_PATH);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sun_family = AF_LOCAL;
	strncpy(serv_addr.sun_path, SERVER_PATH, sizeof(serv_addr.sun_path));

	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("bind");
		clean(1);
	}

	if (listen(sockfd, SIZE_LISTEN) < 0)
	{
		perror("listen");
		clean(1);
	}

	printf("Сервер ожидавет сообщения.\n");
	len_client_addr = sizeof(client_addr);
	new_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &len_client_addr);
	if (new_sockfd < 0)
	{
		perror("accept");
		clean(1);
	}

	byte_in = recv(new_sockfd, buffer, sizeof(buffer) - 1, 0);
	if (byte_in == -1)
	{
		perror("recv");
		clean(1);
	}
	buffer[byte_in] = '\0';
	printf("Пришло сообщение: %s\n", buffer);

	buffer[0] = '0';

	if (send(new_sockfd, buffer, strlen(buffer), 0) == -1)
	{
		perror("send");
		clean(1);
	}
	printf("Отправлено измененное сообщение: %s\n", buffer);

	close(new_sockfd);
	close(sockfd);
	unlink(SERVER_PATH);

	return 0;
}