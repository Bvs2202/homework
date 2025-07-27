#include "server_parse.h"


int hex_to_int(char symbol) {
  if ('0' <= symbol && symbol <= '9')
    return symbol - '0';

  if ('a' <= tolower(symbol) && tolower(symbol) <= 'f')
    return symbol - 'a' + DIGIT_NUMBER;

  return EXIT_ERROR;
}

int parse_mac(const char *str, unsigned char *mac) {
  int index = 0;

  while (index < ETH_ALEN) {
    int high_byte = hex_to_int(*str++);

    if (*str == '\0')
      return EXIT_ERROR;

    int low_byte = hex_to_int(*str++);

    if (high_byte < 0 || low_byte < 0)
      return EXIT_ERROR;

    mac[index++] = (high_byte << MASK_4_BIT) | low_byte;

    if (index < ETH_ALEN && *str != ':')
      return EXIT_ERROR;

    if (index < ETH_ALEN)
      str++;
  }

  return EXIT_SUCCESS;
}

int controller_config(char *filename, char *iface,
                      struct Controller *controllers) {
  int fd = open(filename, O_RDONLY);

  if (fd < 0)
    return -1;

  char buf[SIZE_FILE];
  int index_file = read(fd, buf, sizeof(buf) - 1);

  close(fd);

  if (index_file <= 0)
    return -1;

  buf[index_file] = '\0';

  int len_iface = strlen("iface="), len_ip = strlen("ip="),
      len_mac = strlen("mac="), count_device = 0, iface_flag = 0;
  static uint32_t states[MAX_CONTROLLERS] = {0};
  char *line = strtok(buf, "\n");

  while (line && count_device < MAX_CONTROLLERS) {
    if (strncmp(line, "iface=", len_iface) == 0 && !iface_flag) {
      strncpy(iface, line + len_iface, SIZE_NAME_IFACE - 1);
      iface[SIZE_NAME_IFACE - 1] = '\0';
      iface_flag = 1;
    } else if (strncmp(line, "mac=", len_mac) == 0) {
      parse_mac(line + len_mac, controllers[count_device].mac_dest);
    } else if (strncmp(line, "ip=", len_ip) == 0) {
      controllers[count_device].ip = inet_addr(line + len_ip);
      controllers[count_device].state = &states[count_device];
      count_device++;
    }

    line = strtok(NULL, "\n");
  }

  return count_device;
}