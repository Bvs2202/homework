#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>

#define IP "127.0.0.1"
#define PORT_SERVER 8080
#define PORT_CLIENT 7777

#define SIZE_BUFF 256

int sockfd = -1;

struct header_ip {
	u_int8_t version_len;
	u_int8_t ds;
	u_int16_t total_len;
	u_int16_t id;
	u_int16_t flags_offset;
	u_int8_t ttl;
	u_int8_t transport_proto;
	u_int16_t checksum;
	u_int32_t source_ip;
	u_int32_t dest_ip;
};

struct header_udp {
	u_int16_t source_port;
	u_int16_t dest_port;
	u_int16_t length;
	u_int16_t checksum;
};

struct packet {
	struct header_ip header_ip;
	struct header_udp header_udp;
	char buff[SIZE_BUFF];
};

void clean(int signum)
{
	if (sockfd != -1)
	{
		close(sockfd);
	}
	printf("\n");

	if (signum == 1)
		exit(1);
	else
		exit(0);
}

int main()
{
	struct sockaddr_in addr_client;
	struct sockaddr_in addr_server;
	struct packet pct_send = {0};
	struct packet* pct_in;
	ssize_t byte_in;
	socklen_t addr_len;
	char buff[SIZE_BUFF] = {0};
	int optval = 1;

	signal(SIGINT, clean);

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (sockfd == -1)
	{
		perror("socket");
		exit(1);
	}

	if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) == -1)
	{
		perror("setsockopt");
		clean(1);
	}

	strncpy(pct_send.buff, "Hello world!", sizeof(pct_send.buff));
	pct_send.header_ip.version_len = 4 << 4 | 5;
	pct_send.header_ip.ds = 0;
	pct_send.header_ip.total_len = htons(sizeof(pct_send.header_ip) + sizeof(pct_send.header_udp) + strlen(pct_send.buff));
	pct_send.header_ip.id = 0;
	pct_send.header_ip.flags_offset = 0;
	pct_send.header_ip.ttl = 255;
	pct_send.header_ip.transport_proto = IPPROTO_UDP;
	pct_send.header_ip.checksum = 0;
	if (inet_pton(AF_INET, IP, &pct_send.header_ip.source_ip) <= 0)
	{
		perror("inet_pton");
		clean(1);
	}
	if (inet_pton(AF_INET, IP, &pct_send.header_ip.dest_ip) <= 0)
	{
		perror("inet_pton");
		clean(1);
	}

	pct_send.header_udp.source_port = htons(7777);
	pct_send.header_udp.dest_port = htons(PORT_SERVER);
	pct_send.header_udp.length = htons(sizeof(pct_send.header_udp) + strlen(pct_send.buff));

	addr_server.sin_family = AF_INET;
	addr_server.sin_port = htons(PORT_SERVER);
	if (inet_pton(AF_INET, IP, &addr_server.sin_addr) <= 0)
	{
		perror("inet_pton");
		clean(1);
	}

	sendto(sockfd, &pct_send, sizeof(pct_send), 0, (struct sockaddr *)&addr_server, sizeof(addr_server));

	addr_len = sizeof(addr_client);
	while(1)
	{
		byte_in = recvfrom(sockfd, &buff, sizeof(buff), 0, (struct sockaddr *)&addr_client, &addr_len);
		pct_in = (struct packet *)(buff);
		if (ntohs(pct_in->header_udp.dest_port) == PORT_CLIENT)
		{
			printf("%s\n", pct_in->buff);
		}
	}


	return 0;
}