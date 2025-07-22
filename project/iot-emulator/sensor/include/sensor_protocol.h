#ifndef SENSOR_PROTOCOL_H
#define SENSOR_PROTOCOL_H

#include <stdio.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

// #define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); } while(0)

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
}__attribute__((packed));

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
}__attribute__((packed));

union TransferHead {
	struct UDP udp;
	struct TCP tcp;
}__attribute__((packed));

#pragma pack(push, 1)
struct packet {
	struct ETH eth;
	struct IP ip;
	union TransferHead proto;
	char payload[SIZE_PAYLOAD];
};
#pragma pack(pop)

#endif
