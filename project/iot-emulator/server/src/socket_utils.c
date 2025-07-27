#include "server_parse.h"
#include "smart_home.h"

unsigned short checksum(struct Ip *ip) {
  unsigned short *ptr = (unsigned short *)ip;
  unsigned int csum = 0;

  for (int i = 0; i < IP_HEADER_WORD; i++) {
    csum += ptr[i];
  }

  csum = (csum >> MASK_16_SHIFT) + (csum & 0xFFFF);
  csum += (csum >> MASK_16_SHIFT);

  return (unsigned short)(~csum);
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

int get_interface_mac(const char *interface, unsigned char *mac) {
  struct ifreq ifr;
  int sock;

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    return -1;
  }

  memset(&ifr, 0, sizeof(ifr));
  strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);

  if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
    close(sock);
    return EXIT_ERROR;
  }

  memcpy(mac, ifr.ifr_hwaddr.sa_data, ETH_ALEN);

  close(sock);
  return 0;
}