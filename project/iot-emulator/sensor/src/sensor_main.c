#include "../include/sensor_protocol.h"
#include "../include/config.h"
#include "../include/net_utils.h"
#include "../include/payload.h"

#define SEND_INTERVAL_SEC 1
#define ADC_DELAY 500000
#define RETRY_SEND_SEC 2

#define PORT_SENSOR 6666
#define PORT_SERVER 10000


int main()
{
	char ip_source[INET_ADDRSTRLEN] = {0};
	unsigned char source_mac[ETH_ALEN] = {0};
	struct sensor_config config = {0};
	struct sockaddr_ll sa = {0};
	struct packet packet_send = {0};
	socklen_t len_packet = 0;
	ssize_t send_res = 0;
	int sockfd = 0;
	char *config_path = "config.txt";

	srand(time(NULL));

	sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sockfd == -1)
	{
		perror("socket");
		exit(1);
	}

	read_config(config_path, &config);

	if (get_interface_mac(config.name_iface, source_mac) == -1)
	{
		exit(1);
	}

	if (get_interface_ip(config.name_iface, ip_source) == -1)
	{
		exit(1);
	}


	memset(&sa, 0, sizeof(sa));
	sa.sll_family = AF_PACKET;
	sa.sll_halen = ETH_ALEN;
	sa.sll_ifindex = if_nametoindex(config.name_iface);
	memcpy(sa.sll_addr, config.mac_dest, ETH_ALEN);

	while (1)
	{
		memset(&packet_send, 0, sizeof(packet_send));

		usleep(ADC_DELAY);
		build_payload(packet_send.payload, config.name_sensor);

		memcpy(packet_send.eth.h_dest, config.mac_dest, ETH_ALEN);
		memcpy(packet_send.eth.h_source, source_mac, ETH_ALEN);
		packet_send.eth.h_proto = htons(ETH_P_IP);

		packet_send.ip.version = 4;
		packet_send.ip.ihl = 5;
		packet_send.ip.tos = 0;
		packet_send.ip.tot_len = htons(sizeof(packet_send.ip) + sizeof(packet_send.proto.udp) + strlen(packet_send.payload));
		packet_send.ip.id = 0;
		packet_send.ip.frag_off = 0;
		packet_send.ip.ttl = 255;
		packet_send.ip.protocol = IPPROTO_UDP;
		if (inet_pton(AF_INET, ip_source, &packet_send.ip.saddr) <= 0)
		{
			perror("inet_pton");
			continue;
		}
		if (inet_pton(AF_INET, config.ip_dest, &packet_send.ip.daddr) <= 0)
		{
			perror("inet_pton");
			continue;
		}
		packet_send.ip.check = ip_checksum(&packet_send.ip);

		packet_send.proto.udp.source = htons(PORT_SENSOR);
		packet_send.proto.udp.dest = htons(PORT_SERVER);
		packet_send.proto.udp.len = htons(sizeof(packet_send.proto.udp) + strlen(packet_send.payload));
		packet_send.proto.udp.check = 0;

		len_packet = sizeof(struct ETH) + sizeof(struct IP) + sizeof(union TransferHead) + strlen(packet_send.payload);

		send_res = sendto(sockfd, &packet_send, len_packet, 0, (struct sockaddr *)&sa, sizeof(sa));
		if (send_res == -1)
		{
			sleep(RETRY_SEND_SEC);
		}

		sleep(SEND_INTERVAL_SEC);
	}

	return 0;
}