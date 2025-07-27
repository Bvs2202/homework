#ifndef SMART_HOME_PROTOCOL_H
#define SMART_HOME_PROTOCOL_H

#include <arpa/inet.h>
#include <ctype.h>
#include <fcntl.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netpacket/packet.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define ON_LIGHT 0
#define OFF_LIGHT 1
#define ON_ALL 2
#define OFF_ALL 3
#define SIZE_PAYLOAD 128

struct Eth {
  unsigned char h_dest[ETH_ALEN];
  unsigned char h_source[ETH_ALEN];
  uint16_t h_proto;
} __attribute__((packed));

struct Ip {
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

struct Tcp {
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

struct Udp {
  uint16_t source;
  uint16_t dest;
  uint16_t len;
  uint16_t check;
};

union Transfer_head {
  struct Udp udp;
  struct Tcp tcp;
};

#pragma pack(push, 1)
struct Packet {
  struct Eth eth;
  struct Ip ip;
  union Transfer_head proto;
  unsigned char payload[SIZE_PAYLOAD];
};
#pragma pack(pop)

unsigned short checksum(struct Ip *);
void swap_mac_address(struct Packet *);
void swap_ip_address(struct Packet *);
int get_interface_mac(const char *, unsigned char *);
void swap_tcp_ports(struct Packet *pkt);

#endif
