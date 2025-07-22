#include "../include/config.h"

int read_config(const char *config_path, struct sensor_config *config)
{
	unsigned char mac_str[SIZE_MAC_STR] = {0};
	FILE *fp = fopen(config_path, "r");
	if (fp == NULL)
	{
		perror("fopen");
		return -1;
	}

	if (fscanf(fp, "%15s %15s %15s %17s",
		config->name_sensor, config->name_iface, config->ip_dest, mac_str) != 4)\
	{
		fclose(fp);
		return -1;
	}
	fclose(fp);

	if (sscanf(mac_str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
		&config->mac_dest[0], &config->mac_dest[1], &config->mac_dest[2],
		&config->mac_dest[3], &config->mac_dest[4], &config->mac_dest[5]) != 6)
	{
		return -1;
	}

	return 0;
}