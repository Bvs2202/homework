#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include "../../common/smart_home.h"

#define CONF_FILE_NAME "controller.conf"
#define SIZE_NAME_IFACE 32
#define SIZE_DEVICE_NAME 32
#define SIZE_FILE 1024
#define IP_HEADER_WORD 10
#define MASK_16_SHIFT 16
#define MASK_4_BIT 4
#define ETH_PROTOCOL 0x0800
#define FLAG_PACKET 0

struct Controller {
  uint32_t ip;
  uint32_t *state;
  unsigned char mac_dest[ETH_ALEN];
  char iface[SIZE_NAME_IFACE];
  char device_id[SIZE_DEVICE_NAME];
};

int hex_to_int(char symbol);
int parse_mac(const char *str, unsigned char *mac);
int get_address(const char *line, char *out);
int controller_config(char *filename, char *iface,
                      struct Controller *controllers);

#endif