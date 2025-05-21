#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>

#define SIZE_BUFF 256

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
	struct sockaddr_in addr;
	char buff[SIZE_BUFF] = {0};
	ssize_t byte_in;
	socklen_t len_addr;

	signal(SIGINT, clean);

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	len_addr = sizeof(addr);

	while(1)
	{
		byte_in = recvfrom(sockfd, buff, sizeof(buff) - 1, 0, (struct sockaddr *)&addr, &len_addr);
		if (byte_in == -1)
		{
			perror("recv");
			clean(1);
		}
		buff[byte_in] = '\0';
		for (int i = 28; i < byte_in; i++) {
			printf("%02X ", (unsigned char)buff[i]);
		}
		printf("\n");
		
	}

	return 0;
}