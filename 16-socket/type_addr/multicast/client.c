#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 7777
#define IP "224.0.0.1"

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
	struct ip_mreqn ip_mreqn;
	struct sockaddr_in addr;
	char buff[SIZE_BUFF] = {0};
	socklen_t len_addr;
	ssize_t byte_in;
	sigset_t set;
	struct sigaction sa;

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
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (inet_pton(AF_INET, IP, &ip_mreqn.imr_multiaddr) != 1)
	{
		perror("inet pton");
		clean(0);
	}
	ip_mreqn.imr_address.s_addr = htonl(INADDR_ANY);
	ip_mreqn.imr_ifindex = 0;

	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
		clean(0);
	}

	if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &ip_mreqn, sizeof(ip_mreqn)) == -1)
	{
		perror("setsockopt");
		clean(0);
	}

	len_addr = sizeof(addr);
	byte_in = recvfrom(sockfd, buff, sizeof(buff) - 1, 0, (struct sockaddr *)&addr, &len_addr);
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