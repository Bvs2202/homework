#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 7777

#define SIZE_BUFF 256

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
	sigset_t set;
	struct sigaction sa;
	socklen_t addr_len;
	ssize_t byte_in;
	char buff[SIZE_BUFF] = {0};

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
	if (sockfd == -1)
	{
		perror("socket");
		exit(1);
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT);

	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
		clean(0);
	}

	addr_len = sizeof(addr);
	byte_in = recvfrom(sockfd, buff, sizeof(buff) - 1, 0, (struct sockaddr *)&addr, &addr_len);
	if (byte_in == -1)
	{
		perror("recfrom");
		clean(0);
	}
	buff[byte_in] = '\0';

	printf("%s\n", buff);

	close(sockfd);

	return 0;
}