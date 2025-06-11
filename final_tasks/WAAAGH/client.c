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

#include "common.h"

#define IP_DEST "192.168.0.153"
#define IP_SORCE "192.168.0.197"

int sockfd = -1;

int main()
{
	struct packet packet_send = {0};
	struct packet *packet_in;
	struct sockaddr_ll server_addr, client_addr;
	socklen_t len_addr;
	socklen_t len_packet;
	int byte_in;
	char buff[SIZE_BUFF] = {0};
	char *name_iface = "enp34s0";
	unsigned char dest_mac[SIZE_MAC] = {0x0c, 0x8b, 0xfd, 0x05, 0xed, 0xf3};
	unsigned char source_mac[SIZE_MAC] = {0x04, 0x7c, 0x16, 0xb4, 0xb7, 0xc2};
	unsigned short *ptr_pct;
	unsigned int csum = 0, tmp_csum = 0;

	sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sockfd == -1)
	{
		perror("socket");
		exit(1);
	}

	while (1)
	{
		printf("Введите сообщение для сервера(нажмите 'q' для выхода): ");
		fgets(packet_send.buff, sizeof(packet_send.buff), stdin);
		packet_send.buff[strcspn(packet_send.buff, "\n")] = '\0';

		printf("%s\n", packet_send.buff);

		memcpy(packet_send.header_eathernet.dest_mac, dest_mac, SIZE_MAC);
		memcpy(packet_send.header_eathernet.source_mac, source_mac, SIZE_MAC);
		packet_send.header_eathernet.type = htons(ETH_P_IP);

		packet_send.header_ip.version_len = 4 << 4 | 5;
		packet_send.header_ip.ds = 0;
		packet_send.header_ip.total_len = htons(sizeof(packet_send.header_ip) + sizeof(packet_send.header_udp) + strlen(packet_send.buff));
		packet_send.header_ip.id = 0;
		packet_send.header_ip.flags_offset = 0;
		packet_send.header_ip.ttl = 255;
		packet_send.header_ip.transport_proto = IPPROTO_UDP;
		if (inet_pton(AF_INET, IP_SORCE, &packet_send.header_ip.source_ip) <= 0)
		{
			perror("inet_pton");
			return 1;
		}
		if (inet_pton(AF_INET, IP_DEST, &packet_send.header_ip.dest_ip) <= 0)
		{
			perror("inet_pton");
			return 1;
		}
		ptr_pct = (unsigned short *)&packet_send.header_ip;
		csum = 0; tmp_csum = 0;
		for (int i = 0; i < 10; i++)
		{
			csum = csum + ptr_pct[i];
		}
		tmp_csum = csum >> 16;
		csum = (csum & 0xFFFF) + tmp_csum;
		csum = ~csum;
		packet_send.header_ip.checksum = (csum & 0xFFFF);

		packet_send.header_udp.source_port = htons(7777);
		packet_send.header_udp.dest_port = htons(PORT_SERVER);
		packet_send.header_udp.length = htons(sizeof(packet_send.header_udp) + strlen(packet_send.buff));

		server_addr.sll_family = AF_PACKET;
		server_addr.sll_halen = SIZE_ADDR_SLL;
		memcpy(server_addr.sll_addr, dest_mac, SIZE_MAC);
		server_addr.sll_ifindex = if_nametoindex(name_iface);


		len_packet = sizeof(struct header_eathernet) + sizeof(struct header_ip) + sizeof(struct header_udp) + strlen(packet_send.buff);
		sendto(sockfd, &packet_send, len_packet, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

		if (strcmp(packet_send.buff, "q") == 0)
		{
			break;
		}

		len_addr = sizeof(server_addr);
		while(1)
		{
			byte_in = recvfrom(sockfd, &buff, sizeof(buff), 0, (struct sockaddr *)&server_addr, &len_addr);
			packet_in = (struct packet *)(buff);
			if (ntohs(packet_in->header_udp.dest_port) == PORT_CLIENT)
			{
				printf("%s\n", packet_in->buff);
				break;
			}
		}
	}

	close(sockfd);

	return 0;
}