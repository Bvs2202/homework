#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/un.h>
#include <unistd.h>

#define SIZE_BUFF 256

#define SERVER_PATH "/tmp/my_server_sock"
#define CLIENT_PATH "/tmp/my_client_sock"

int sockfd = -1;

void clean(int signum)
{
	if (sockfd != -1)
	{
		close(sockfd);
	}
	unlink(CLIENT_PATH);
	printf("\n");

	exit(0);
}

int main()
{
	struct sockaddr_un serv_addr, client_addr;
	char *msg = "Hello world!";
	char buffer[SIZE_BUFF] = {0};
	ssize_t byte_in;
	struct sigaction sa;
	sigset_t set;
	socklen_t addr_len;

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

	sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		return 1;
	}

	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sun_family = AF_LOCAL;
	strncpy(client_addr.sun_path, CLIENT_PATH, sizeof(client_addr.sun_path));
	if (bind(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
	{
		perror("bind");
		clean(1);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sun_family = AF_LOCAL;
	strncpy(serv_addr.sun_path, SERVER_PATH, sizeof(serv_addr.sun_path));

	if (sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
	{
		perror("send");
		clean(1);
	}
	printf("Отправлено сообщение: %s\n", msg);

	addr_len = sizeof(client_addr);
	byte_in = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&client_addr, &addr_len);
	if (byte_in == -1)
	{
		perror("recv");
		clean(1);
	}
	buffer[byte_in] = '\0';

	printf("Пришло сообщение от сервера: %s\n", buffer);

	close(sockfd);
	unlink(CLIENT_PATH);

	return 0;
}