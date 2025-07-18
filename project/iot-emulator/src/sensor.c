#include "../include/smart_home.h"

#define NAME_SENSOR "sensor1"
#define NAME_IFACE "enp34s0"
#define IP_DEST "192.168.0.153"
#define IP_SOURCE "192.168.0.197"
#define PORT_SENSOR 6666
#define PORT_SERVER 7777
#define SIZE_ADDR_SLL 6

float generate_temparature(void)
{
	return 20.0 + ((rand() % 100)) * 0.1;
}

void time_to_str(time_t ts, char* ptr)
{
	for (int i = 9; i >= 0; i--)
	{
		ptr[i] = '0' + (ts % 10);
		ts /= 10;
	}
}

void float_to_str(float temp, char *ptr)
{
	int integer = (int)temp;
	int frac = (int)((temp - integer) * 10);

	*ptr++ = '0' + integer / 10;
	*ptr++ = '0' + integer % 10;
	*ptr++ = '.';
	*ptr++ = '0' + frac;
}

void build_payload(char *buff)
{
	time_t ts = time(NULL);
	float temp = generate_temparature();
	char *ptr = buff;

	*ptr = '{';
	ptr++;

	memcpy(ptr, "\"dev\":\"", strlen("\"dev\":\""));
	ptr += strlen("\"dev\":\"");

	memcpy(ptr, NAME_SENSOR, strlen(NAME_SENSOR));
	ptr += strlen(NAME_SENSOR);
	*ptr = '\"';
	ptr++;
	
	memcpy(ptr, ", \"val\":", strlen(", \"val\":"));
	ptr += strlen(", \"val\":");
	float_to_str(temp, ptr);
	ptr += strlen(ptr);

	memcpy(ptr, ", \"ts\":", strlen(", \"ts\":"));
	ptr += strlen(", \"ts\":");
	time_to_str(ts, ptr);
	ptr += 10;

	*ptr = '}';
	ptr++;
	*ptr = '\0';
}

int main()
{
	struct sockaddr_ll sa = {0};
	struct packet packet_send = {0};
	socklen_t len_packet = 0;
	int sockfd = 0;
	unsigned char dest_mac[ETH_ALEN] = {0x0c, 0x8b, 0xfd, 0x05, 0xed, 0xf3};
	unsigned char source_mac[ETH_ALEN] = {0};
	float temp = 0;
	time_t ts = 0;

	srand(time(NULL));

	get_interface_mac(NAME_IFACE, source_mac);

	sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sockfd == -1)
	{
		perror("socket");
		exit(1);
	}

	while (1)
	{
		memset(&sa, 0, sizeof(sa));
		memset(&packet_send, 0, sizeof(packet_send));

		build_payload(packet_send.payload);

		memcpy(packet_send.eth.h_dest, dest_mac, ETH_ALEN);
		memcpy(packet_send.eth.h_source, source_mac, ETH_ALEN);
		packet_send.eth.h_proto = htons(ETH_P_IP);

		packet_send.ip.version = 4;
		packet_send.ip.ihl = 5;
		packet_send.ip.tos = 0;
		packet_send.ip.tot_len = htons(sizeof(packet_send.ip) + sizeof(packet_send.proto.udp) + SIZE_PAYLOAD);
		packet_send.ip.id = 0;
		packet_send.ip.frag_off = 0;
		packet_send.ip.ttl = 255;
		packet_send.ip.protocol = IPPROTO_UDP;
		if (inet_pton(AF_INET, IP_SOURCE, &packet_send.ip.saddr) <= 0)
		{
			perror("inet_pton");
			return 1;
		}
		if (inet_pton(AF_INET, IP_DEST, &packet_send.ip.daddr) <= 0)
		{
			perror("inet_pton");
			return 1;
		}

		packet_send.ip.check = checksum(&packet_send.ip);

		packet_send.proto.udp.source = htons(PORT_SENSOR);
		packet_send.proto.udp.dest = htons(PORT_SERVER);
		packet_send.proto.udp.len = htons(sizeof(packet_send.proto.udp) + SIZE_PAYLOAD);
		packet_send.proto.udp.check = 0;

		sa.sll_family = AF_PACKET;
		sa.sll_halen = SIZE_ADDR_SLL;
		memcpy(sa.sll_addr, dest_mac, ETH_ALEN);
		sa.sll_ifindex = if_nametoindex(NAME_IFACE);

		len_packet = sizeof(struct ETH) + sizeof(struct IP) + sizeof(union TransferHead) + SIZE_PAYLOAD;

		sendto(sockfd, &packet_send, len_packet, 0, (struct sockaddr *)&sa, sizeof(sa));

		sleep(1);
	}

	return 0;
}