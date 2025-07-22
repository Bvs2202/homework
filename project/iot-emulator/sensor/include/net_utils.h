#ifndef NET_UTILS_H
#define NET_UTILS_H

#include <sys/socket.h>
#include <net/ethernet.h>

#include "sensor_protocol.h"

int  get_interface_ip(const char *iface, char *ip_str);
int  get_interface_mac(const char *iface, unsigned char *mac);
int  create_raw_socket(void);
unsigned short ip_checksum(const struct IP *ip_header);

#endif
