#include "../include/net_utils.h"
#include "../../common/str_utils.h"

int get_interface_ip(const char *interface, char *ip_str) {
  struct ifreq ifr = {0};
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1) return -1;

  strncpy(ifr.ifr_name, interface, IFNAMSIZ);
  if (ioctl(sock, SIOCGIFADDR, &ifr) < 0) {
    close(sock);
    return -1;
  }

  struct sockaddr_in *ipaddr = (struct sockaddr_in *)&ifr.ifr_addr;
  unsigned char *ip = (unsigned char *)&ipaddr->sin_addr.s_addr;

  snprintj(ip_str, INET_ADDRSTRLEN, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

  close(sock);

  return 0;
}

int get_interface_mac(const char *interface, unsigned char *mac) {
  struct ifreq ifr = {0};
  int sock;

  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    return -1;
  }

  strncpy(ifr.ifr_name, interface, IFNAMSIZ);
  if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
    close(sock);
    return -1;
  }

  memcpy(mac, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
  close(sock);

  return 0;
}

unsigned short ip_checksum(const struct Ip *ip_header) {
  unsigned short *ptr = (unsigned short *)ip_header;
  unsigned int csum = 0;

  for (int i = 0; i < 10; i++) {
    csum += ptr[i];
  }

  unsigned short tmp = csum >> 16;
  csum = (csum & 0xFFFF) + tmp;

  return ~(unsigned short)csum;
}
