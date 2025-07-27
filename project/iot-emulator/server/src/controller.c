#include "server_parse.h"

struct Controller *get_controller(const char *device_id,
                                  struct Controller *controllers,
                                  int *count_device) {
  for (int n = 0; n < *count_device; n++)
    if (strncmp(controllers[n].device_id, device_id, SIZE_NAME_IFACE) == 0)
      return &controllers[n];

  for (int n = 0; n < *count_device; n++)
    if (controllers[n].device_id[0] == '\0') {
      strncpy(controllers[n].device_id, device_id, SIZE_NAME_IFACE - 1);
      controllers[n].device_id[SIZE_NAME_IFACE - 1] = '\0';
      return &controllers[n];
    }

  if (*count_device >= MAX_CONTROLLERS)
    return NULL;

  struct Controller *base = &controllers[0];
  struct Controller *contr = &controllers[*count_device];

  memcpy(contr, base, sizeof(struct Controller));

  contr->state = base->state;

  strncpy(contr->device_id, device_id, SIZE_NAME_IFACE - 1);

  contr->device_id[SIZE_NAME_IFACE - 1] = '\0';

  (*count_device)++;

  return contr;
}