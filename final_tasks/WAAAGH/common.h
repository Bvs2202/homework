#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>

#define SIZE_ADDR_SLL 6
#define SIZE_IP 4
#define SIZE_MAC 6
#define PORT_SERVER 8080
#define PORT_CLIENT 7777

#define EXIT_CLIENT "q"

#define SIZE_BUFF 256

struct header_ethernet {
	unsigned char dst_mac[SIZE_MAC];
	unsigned char src_mac[SIZE_MAC];
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
	u_int16_t ip_checksum;
	u_int32_t source_ip;
	u_int32_t dest_ip;
}__attribute__((packed));

struct header_udp {
	u_int16_t source_port;
	u_int16_t dest_port;
	u_int16_t length;
	u_int16_t udp_checksum;
}__attribute__((packed));

struct ethernet_frame {
	struct header_ethernet header_ethernet;
	struct header_ip header_ip;
	struct header_udp header_udp;
	char buff[SIZE_BUFF];
};

#endif
