#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SIZE_BUFF 256

#define PORT_SERVER 7777
#define SIZE_PAYLOAD 64

struct ETH {
	unsigned char h_dest[ETH_ALEN];
	unsigned char h_source[ETH_ALEN];
	uint16_t h_proto;
} __attribute__((packed));

struct IP {
	uint8_t ihl : 4, version : 4;
	uint8_t tos;
	uint16_t tot_len;
	uint16_t id;
	uint16_t frag_off;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t check;
	uint32_t saddr;
	uint32_t daddr;
};

struct TCP {
	uint16_t source;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack_seq;
	uint8_t data_off;
	uint8_t flags;
	uint16_t window;
	uint16_t check;
	uint16_t urg_ptr;
} __attribute__((packed));

struct UDP {
	uint16_t source;
	uint16_t dest;
	uint16_t len;
	uint16_t check;
};

union TransferHead {
	struct UDP udp;
	struct TCP tcp;
};

#pragma pack(push, 1)
struct packet {
	struct ETH eth;
	struct IP ip;
	union TransferHead proto;
	char payload[SIZE_PAYLOAD];
};
#pragma pack(pop)


int sockfd = -1;


int main()
{
	char buff[SIZE_BUFF] = {0};
	struct packet *packet;
	ssize_t byte_in;
	socklen_t addr_len;

	int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

	while (1)
	{
		memset(buff, 0, sizeof(buff));
	    ssize_t n = recvfrom(sock, buff, SIZE_BUFF, 0, NULL, NULL);

		packet = (struct packet *)(buff);

		if (ntohs(packet->proto.udp.dest) == PORT_SERVER)
		{
			printf("%s\n", packet->payload);
		}
	}

	close(sockfd);

	return 0;
}



smart_home
|___sensor
|	├── include/
|	│   ├── smart_home_protocol.h
|	│   ├── config.h
|	│   ├── net_utils.h
|	│   ├── payload.h
|	│   └── checksum.h
|	├── src/
|	│   ├── config.c
|	│   ├── net_utils.c
|	│   ├── payload.c
|	│   ├── checksum.c
|	│   └── main.c
|	└── Makefile
|
|__serever
	├── include/
	├── src/
	└── Makefile