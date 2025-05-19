#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 7777
#define IP "255.255.255.255"

int sockfd = -1;

void clean(int signum)
{
	if (sockfd != -1)
	{
		close(sockfd);
	}
	printf("\n");

	exit(1);
}

int main()
{
	struct sockaddr_in addr;
	char *msg = "Hello client!";
	sigset_t set;
	struct sigaction sa;
	int flag_socket = 1;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sa.sa_handler = clean;
	sa.sa_mask = set;
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) < 0)
	{
		perror("sigaction");
		clean(0);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	addr.sin_family = AF_INET;
	if (inet_pton(AF_INET, IP, &addr.sin_addr) != 1)
	{
		perror("inet_pton");
		clean(0);
	}
	addr.sin_port = htons(PORT);
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &flag_socket, sizeof(flag_socket)) == -1)
	{
		perror("setsockopt");
		clean(0);
	}

	if (sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("sendto");
		clean(0);
	}

	printf("Нажмите любую клавишу для выхода: ");
	getchar();
	printf("\n");

	close(sockfd);

	return 0;
}