#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netpacket/packet.h>
#include <net/if.h>

#include "common.h"

#define IP_DEST "192.168.0.153"
#define IP_SOURCE "192.168.0.197"

int raw_sock_fd = -1;

int main()
{
	struct ethernet_frame tx_pkt = {0};
	struct ethernet_frame *rx_pkt;
	struct sockaddr_ll peer_addr;
	socklen_t peer_addr_len;
	socklen_t len_packet;
	int recv_len;
	char recv_buff[SIZE_BUFF] = {0};
	char *name_iface = "enp34s0";
	unsigned char dst_mac[SIZE_MAC] = {0x0c, 0x8b, 0xfd, 0x05, 0xed, 0xf3};
	unsigned char src_mac[SIZE_MAC] = {0x04, 0x7c, 0x16, 0xb4, 0xb7, 0xc2};
	unsigned short *ip_hdr_ptr;
	unsigned int csum = 0, tmp_csum = 0;

	raw_sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (raw_sock_fd == -1)
	{
		perror("socket");
		exit(1);
	}

	while (1)
	{
		memset(&tx_pkt, 0, sizeof(tx_pkt));
		printf("Введите сообщение для сервера(нажмите 'q' для выхода): ");
		fgets(tx_pkt.buff, sizeof(tx_pkt.buff), stdin);
		tx_pkt.buff[strcspn(tx_pkt.buff, "\n")] = '\0';

		memcpy(tx_pkt.header_ethernet.dst_mac, dst_mac, SIZE_MAC);
		memcpy(tx_pkt.header_ethernet.src_mac, src_mac, SIZE_MAC);
		tx_pkt.header_ethernet.type = htons(ETH_P_IP);

		tx_pkt.header_ip.version_len = 4 << 4 | 5;
		tx_pkt.header_ip.ds = 0;
		tx_pkt.header_ip.total_len = htons(sizeof(tx_pkt.header_ip) + sizeof(tx_pkt.header_udp) + strlen(tx_pkt.buff));
		tx_pkt.header_ip.id = 0;
		tx_pkt.header_ip.flags_offset = 0;
		tx_pkt.header_ip.ttl = 255;
		tx_pkt.header_ip.transport_proto = IPPROTO_UDP;
		if (inet_pton(AF_INET, IP_SOURCE, &tx_pkt.header_ip.source_ip) <= 0)
		{
			perror("inet_pton");
			return 1;
		}
		if (inet_pton(AF_INET, IP_DEST, &tx_pkt.header_ip.dest_ip) <= 0)
		{
			perror("inet_pton");
			return 1;
		}
		ip_hdr_ptr = (unsigned short *)&tx_pkt.header_ip;
		csum = 0; tmp_csum = 0;
		for (int i = 0; i < 10; i++)
		{
			csum = csum + ip_hdr_ptr[i];
		}
		tmp_csum = csum >> 16;
		csum = (csum & 0xFFFF) + tmp_csum;
		csum = ~csum;
		tx_pkt.header_ip.ip_checksum = (csum & 0xFFFF);

		tx_pkt.header_udp.source_port = htons(7777);
		tx_pkt.header_udp.dest_port = htons(PORT_SERVER);
		tx_pkt.header_udp.length = htons(sizeof(tx_pkt.header_udp) + strlen(tx_pkt.buff));

		peer_addr.sll_family = AF_PACKET;
		peer_addr.sll_halen = SIZE_ADDR_SLL;
		memcpy(peer_addr.sll_addr, dst_mac, SIZE_MAC);
		peer_addr.sll_ifindex = if_nametoindex(name_iface);	


		len_packet = sizeof(struct header_ethernet) + sizeof(struct header_ip) + sizeof(struct header_udp) + strlen(tx_pkt.buff);
		if (sendto(raw_sock_fd, &tx_pkt, len_packet, 0, (struct sockaddr *)&peer_addr, sizeof(peer_addr)) < 0)
		{
			perror("sendto");
			continue;
		}

		if (strcmp(tx_pkt.buff, "q") == 0)
		{
			printf("Завершение работы!\n");
			break;
		}

		peer_addr_len = sizeof(peer_addr);
		while(1)
		{
			recv_len = recvfrom(raw_sock_fd, &recv_buff, sizeof(recv_buff), 0, (struct sockaddr *)&peer_addr, &peer_addr_len);
			rx_pkt = (struct ethernet_frame *)(recv_buff);
			if (ntohs(rx_pkt->header_udp.dest_port) == PORT_CLIENT)
			{
				printf("%s\n", rx_pkt->buff);
				break;
			}
		}
	}

	close(raw_sock_fd);

	return 0;
}