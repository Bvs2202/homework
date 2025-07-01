#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netpacket/packet.h>
#include <netinet/if_ether.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "common.h"

#define IP_SOURCE "192.168.0.153"

#define MAX_CLIENT 36

struct client_data {
	int num;
	u_int32_t ip_client;
	u_int16_t port_client;
};

struct list {
	struct client_data client;
	struct list *prev;
	struct list *next;
};

int sockfd = -1;
volatile sig_atomic_t stop = 0;

void handle_sigint(int signum)
{
	stop = 1;
}

int main()
{
	struct sigaction sa = {0};
	struct client_data *ptr_client_data = NULL;
	struct list *head;
	struct list *ptr_list;
	struct list *new_list;
	struct packet packet_send = {0};
	struct packet *packet_in;
	struct sockaddr_ll server_addr, client_addr;
	socklen_t len_addr;
	socklen_t len_packet;
	int byte_in;
	char buff[SIZE_BUFF] = {0};
	char tmp_buff[SIZE_BUFF] = {0};
	char *name_iface = "wlp4s0";
	unsigned char dest_mac[SIZE_MAC];
	unsigned char source_mac[SIZE_MAC] = {0x0c, 0x8b, 0xfd, 0x05, 0xed, 0xf3};
	unsigned short *ptr_pct;
	unsigned int csum = 0;
	size_t len_str;

	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);

	sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sockfd == -1)
	{
		perror("socket");
		exit(1);
	}

	head = calloc(1, sizeof(struct list));
	if (head == NULL)
	{
		perror("calloc head");
		return 1;
	}
	head->next = head;
	head->prev = head;

	while (stop != 1)
	{
		memset(&packet_send, 0, sizeof(packet_send));

		printf("Ожидаю сообщения от клиента!\n");
		len_addr = sizeof(client_addr);
		while(1)
		{
			ptr_client_data = NULL;

			byte_in = recvfrom(sockfd, &buff, sizeof(buff), 0, (struct sockaddr *)&client_addr, &len_addr);
			if (byte_in < 0)
			{
				if (errno == EINTR)
				{
					break;
				}
				perror("recvfrom");
				continue;
			}

			packet_in = (struct packet *)(buff);
			if (ntohs(packet_in->header_udp.dest_port) == PORT_SERVER)
			{
				printf("port_client = %d, сообщение клиента: %s\n", ntohs(packet_in->header_udp.source_port), packet_in->buff);

				if (strcmp(packet_in->buff, EXIT_CLIENT) == 0)
				{
					ptr_list = head->next;
					while (ptr_list != head)
					{
						if (ptr_list->client.ip_client == ntohl(packet_in->header_ip.source_ip)
							&& ptr_list->client.port_client == ntohs(packet_in->header_udp.source_port))
						{
							ptr_list->prev->next = ptr_list->next;
							ptr_list->next->prev = ptr_list->prev;

							free(ptr_list);
							break;
						}
						ptr_list = ptr_list->next;
					}
					continue;
				}

				for (ptr_list = head->next; ptr_list != head; ptr_list = ptr_list->next)
				{
					if (ptr_list->client.ip_client == ntohl(packet_in->header_ip.source_ip)
							&& ptr_list->client.port_client == ntohs(packet_in->header_udp.source_port))
					{
						ptr_list->client.num++;
						ptr_client_data = &ptr_list->client;
						break;
					}
				}

				if (!ptr_client_data)
				{
					new_list = calloc(1, sizeof(*new_list));
					if (!new_list)
					{
						perror("calloc new list");
						ptr_list = head->next;
						while (ptr_list != head)
						{
							struct list *next = ptr_list->next;
							free(ptr_list);
							ptr_list = next;
						}
					
						free(head);
						return 1;
					}

					new_list->client.ip_client = ntohl(packet_in->header_ip.source_ip);
					new_list->client.port_client = ntohs(packet_in->header_udp.source_port);
					new_list->client.num = 1;

					new_list->next = head;
					new_list->prev = head->prev;
					head->prev->next = new_list;
					head->prev = new_list;

					ptr_client_data = &new_list->client;
				}

				break;
			}
		}
		if (stop == 1)
		{
			ptr_list = head->next;
			while (ptr_list != head)
			{
				struct list *next = ptr_list->next;
				free(ptr_list);
				ptr_list = next;
			}

			free(head);

			break;
		}

		strncpy(packet_send.buff, packet_in->buff, sizeof(packet_send.buff));
		len_str = strlen(packet_send.buff);
		snprintf(packet_send.buff + len_str, SIZE_BUFF - len_str, " %d", ptr_client_data->num);

		memcpy(dest_mac, packet_in->header_eathernet.source_mac, SIZE_MAC);

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
		packet_send.header_ip.dest_ip = packet_in->header_ip.source_ip;

		if (inet_pton(AF_INET, IP_SOURCE, &packet_send.header_ip.source_ip) <= 0)
		{
			perror("inet_pton");
		}
		csum = 0;
		ptr_pct = (unsigned short *)&packet_send.header_ip;
		for (int i = 0; i < 10; i++)
		{
			csum = csum + ptr_pct[i];
		}
		csum = (csum & 0xFFFF) + (csum >> 16);
		csum = ~csum;
		packet_send.header_ip.checksum = (csum & 0xFFFF);

		packet_send.header_udp.source_port = htons(PORT_SERVER);
		packet_send.header_udp.dest_port = packet_in->header_udp.source_port;
		packet_send.header_udp.length = htons(sizeof(packet_send.header_udp) + strlen(packet_send.buff));

		server_addr.sll_family = AF_PACKET;
		server_addr.sll_halen = SIZE_ADDR_SLL;
		memcpy(server_addr.sll_addr, dest_mac, SIZE_MAC);
		server_addr.sll_ifindex = if_nametoindex("wlp4s0");

		len_packet = sizeof(struct header_eathernet) + sizeof(struct header_ip) + sizeof(struct header_udp) + strlen(packet_send.buff);
		sendto(sockfd, &packet_send, len_packet, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

		inet_ntop(AF_INET, &packet_send.header_ip.dest_ip, tmp_buff, INET_ADDRSTRLEN);
		printf("Содержимое пакета: %s %s\n", tmp_buff, packet_send.buff);
	}

	close(sockfd);

	return 0;
}
