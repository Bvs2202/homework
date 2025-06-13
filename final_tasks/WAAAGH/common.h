#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include "common.h"

#define SIZE_ADDR_SLL 6
#define SIZE_IP 4
#define SIZE_MAC 6
#define PORT_SERVER 8080
#define PORT_CLIENT 7777

#define EXIT_CLIENT "q"

#define SIZE_BUFF 256

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

#endif
