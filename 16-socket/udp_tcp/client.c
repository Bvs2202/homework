#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>

#define PORT_SERVER 8080
#define PORT_CLIENT 7777
#define IP_SERVER "127.0.0.1"

#define SIZE_BUFF 256

int sockfd = -1;

struct header {
	u_int16_t source_port;
	u_int16_t dest_port;
	u_int16_t length;
	u_int16_t checksum;
};

struct packet {
	struct header header;
	char buff[SIZE_BUFF];
};

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
	struct packet pct_send = {0};
	struct packet *pct_in;
	struct sockaddr_in addr_client, addr_server;
	ssize_t byte_in;
	socklen_t addr_len;
	char buff[SIZE_BUFF] = {0};

	signal(SIGINT, clean);

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (sockfd == -1)
	{
		perror("socket");
		exit(1);
	}

	strncpy(pct_send.buff, "Hello world!", sizeof(pct_send.buff));
	pct_send.header.source_port = htons(7777);
	pct_send.header.dest_port = htons(PORT_SERVER);
	pct_send.header.length = htons(sizeof(pct_send.header) + strlen(pct_send.buff));

	addr_server.sin_family = AF_INET;
	addr_server.sin_port = htons(PORT_SERVER);
	if (inet_pton(AF_INET, IP_SERVER, &addr_server.sin_addr) <= 0)
	{
		perror("inet_pton");
		clean(1);
	}

	sendto(sockfd, &pct_send, sizeof(pct_send), 0, (struct sockaddr *)&addr_server, sizeof(addr_server));

	addr_len = sizeof(addr_client);
	while(1)
	{
		byte_in = recvfrom(sockfd, &buff, sizeof(buff), 0, (struct sockaddr *)&addr_client, &addr_len);
		pct_in = (struct packet *)(buff + 20);
		if (ntohs(pct_in->header.dest_port) == PORT_CLIENT)
		{
			printf("%s\n", pct_in->buff);
		}
	}
}