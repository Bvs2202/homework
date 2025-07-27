#include "../../common/smart_home.h"
#include "../include/str_utils.h"

unsigned short checksum(struct Ip *ip_header) {
  unsigned short *ptr = (unsigned short *)ip_header;
  unsigned int csum = 0;

  for (int i = 0; i < 10; i++) {
    csum += ptr[i];
  }

  unsigned short tmp = csum >> 16;
  csum = (csum & 0xFFFF) + tmp;
  return ~(unsigned short)csum;
}

void swap_mac_address(struct Packet *pkt) {
  unsigned char tmp_mac[ETH_ALEN];
  memcpy(tmp_mac, pkt->eth.h_dest, ETH_ALEN);
  memcpy(pkt->eth.h_dest, pkt->eth.h_source, ETH_ALEN);
  memcpy(pkt->eth.h_source, tmp_mac, ETH_ALEN);
}

void swap_ip_address(struct Packet *pkt) {
  uint32_t tmp_ip = pkt->ip.daddr;
  pkt->ip.daddr = pkt->ip.saddr;
  pkt->ip.saddr = tmp_ip;
}

void swap_tcp_ports(struct Packet *pkt) {
  uint16_t tmp_port = pkt->proto.tcp.dest;
  pkt->proto.tcp.dest = pkt->proto.tcp.source;
  pkt->proto.tcp.source = tmp_port;
}

int get_interface_mac(const char *interface, unsigned char *mac) {
  struct ifreq ifr;
  int sock;

  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) return -1;

  strncpy(ifr.ifr_name, interface, IFNAMSIZ);
  if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
    close(sock);
    return -1;
  }

  memcpy(mac, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
  close(sock);
  return 0;
}

void encryption(char json_str[SIZE_PAYLOAD]) {
  char *ptr = json_str;
  int i;
  size_t bytes = strlen(json_str);

  for (i = 0; i < bytes; i++) {
    *ptr ^= 0XAA;
    ptr++;
  }
}
