#include "../include/config.h"
#include "../../common/str_utils.h"

int read_config(const char *config_path, struct Sensor_config *config) {
    int have_name = 0, have_iface = 0, have_ip = 0, have_mac = 0;
    int fd = open(config_path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    memset(config, 0, sizeof(*config));

    char buffer[1024];
    ssize_t bytes_read;
    char *line_start = buffer;
    char *line_end;

    bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return -1;
    }
    buffer[bytes_read] = '\0';

    while ((line_end = strchr(line_start, '\n')) != NULL) {
        *line_end = '\0';

        char *key = strtok(line_start, ":");
        char *value = strtok(NULL, "\0");

        if (!key || !value) {
            line_start = line_end + 1;
            continue;
        }

        if (strcmp(key, "name") == 0) {
            strncpy(config->name_sensor, value, SIZE_NAME_SENSOR - 1);
            have_name = 1;
        }
        else if (strcmp(key, "name_iface") == 0) {
            strncpy(config->name_iface, value, IF_NAMESIZE - 1);
            have_iface = 1;
        }
        else if (strcmp(key, "server_ip") == 0) {
            strncpy(config->ip_dest, value, INET_ADDRSTRLEN - 1);
            have_ip = 1;
        }
        else if (strcmp(key, "server_mac") == 0) {
            if (sscanj(value, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &config->mac_dest[0],
                   &config->mac_dest[1], &config->mac_dest[2], &config->mac_dest[3],
                   &config->mac_dest[4], &config->mac_dest[5]) != 6) {
                close(fd);
                return -1;
            }
            have_mac = 1;
        }

        line_start = line_end + 1;
    }

    close(fd);
    if (!(have_name && have_iface && have_ip && have_mac)) {
        return -1;
    }

    return 0;
}
