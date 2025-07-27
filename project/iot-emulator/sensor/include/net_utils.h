#ifndef NET_UTILS_H
#define NET_UTILS_H

#include <net/ethernet.h>
#include <sys/socket.h>

#include "../../common/smart_home.h"

int get_interface_ip(const char *iface, char *ip_str);
int get_interface_mac(const char *iface, unsigned char *mac);
int create_raw_socket(void);
unsigned short ip_checksum(const struct Ip *ip_header);

#endif
