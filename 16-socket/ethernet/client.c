#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>

#define IP_DEST "192.168.0.153"
#define IP_SORCE "192.168.0.197"
#define PORT_SERVER 8080
#define PORT_CLIENT 7777

#define SIZE_BUFF 256
#define SIZE_ADDR_SLL 6
#define SIZE_MAC 6

int sockfd = -1;

struct header_eathernet {
	unsigned char dest_mac[SIZE_MAC];
	unsigned char source_mac[SIZE_MAC];
	u_int16_t type;
}__attribute__((packed));

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
}__attribute__((packed));

struct header_udp {
	u_int16_t source_port;
	u_int16_t dest_port;
	u_int16_t length;
	u_int16_t checksum;
}__attribute__((packed));

struct packet {
	struct header_eathernet header_eathernet;
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
	struct sockaddr_ll addr_client;
	struct sockaddr_ll addr_server;
	struct packet pct_send = {0};
	struct packet* pct_in;
	ssize_t byte_in;
	socklen_t addr_len;
	int len_pct = 0;
	char buff[SIZE_BUFF] = {0};
	char dest_mac[SIZE_MAC] = {0x0c, 0x8b, 0xfd, 0x05, 0xed, 0xf3};
	char source_mac[SIZE_MAC] = {0x04, 0x7c, 0x16, 0xb4, 0xb7, 0xc2};
	unsigned int csum = 0, tmp_csum = 0;
	unsigned short *ptr_pct;

	signal(SIGINT, clean);

	sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sockfd == -1)
	{
		perror("socket");
		exit(1);
	}

	memcpy(pct_send.header_eathernet.dest_mac, dest_mac, SIZE_MAC);
	memcpy(pct_send.header_eathernet.source_mac, source_mac, SIZE_MAC);
	pct_send.header_eathernet.type = htons(ETH_P_IP);

	strncpy(pct_send.buff, "Hello world!", sizeof(pct_send.buff));
	pct_send.header_ip.version_len = 4 << 4 | 5;
	pct_send.header_ip.ds = 0;
	pct_send.header_ip.total_len = htons(sizeof(pct_send.header_ip) + sizeof(pct_send.header_udp) + strlen(pct_send.buff));
	pct_send.header_ip.id = 0;
	pct_send.header_ip.flags_offset = 0;
	pct_send.header_ip.ttl = 255;
	pct_send.header_ip.transport_proto = IPPROTO_UDP;
	if (inet_pton(AF_INET, IP_SORCE, &pct_send.header_ip.source_ip) <= 0)
	{
		perror("inet_pton");
		clean(1);
	}
	if (inet_pton(AF_INET, IP_DEST, &pct_send.header_ip.dest_ip) <= 0)
	{
		perror("inet_pton");
		clean(1);
	}
	ptr_pct = (unsigned short *)&pct_send.header_ip;
	for (int i = 0; i < 10; i++)
	{
		csum = csum + ptr_pct[i];
	}
	tmp_csum = csum >> 16;
	csum = (csum & 0xFFFF) + tmp_csum;
	csum = ~csum;
	pct_send.header_ip.checksum = (csum & 0xFFFF);

	pct_send.header_udp.source_port = htons(7777);
	pct_send.header_udp.dest_port = htons(PORT_SERVER);
	pct_send.header_udp.length = htons(sizeof(pct_send.header_udp) + strlen(pct_send.buff));

	addr_server.sll_family = AF_PACKET;
	addr_server.sll_halen = SIZE_ADDR_SLL;
	memcpy(addr_server.sll_addr, dest_mac, SIZE_MAC);
	addr_server.sll_ifindex = if_nametoindex("enp34s0");

	len_pct = sizeof(struct header_eathernet) + sizeof(struct header_ip) + sizeof(struct header_udp) + strlen(pct_send.buff);
	sendto(sockfd, &pct_send, len_pct, 0, (struct sockaddr *)&addr_server, sizeof(addr_server));

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