#ifndef CONFIG_H
#define CONFIG_H

#include "../../common/smart_home.h"

#define SIZE_NAME_SENSOR 16
#define SIZE_MAC_STR 18

struct Sensor_config {
  char name_sensor[SIZE_NAME_SENSOR];
  char name_iface[IF_NAMESIZE];
  char ip_dest[INET_ADDRSTRLEN];
  unsigned char mac_dest[ETH_ALEN];
};

int read_config(const char *config_path, struct Sensor_config *config);

#endif
