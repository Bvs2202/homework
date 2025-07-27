#include "server_parse.h"


int parse_sensor(const char *str, char *device_id, float *val, int *seq) {
  if (!str || !device_id || !val)
    return EXIT_ERROR;

  char *dev = strstr(str, "\"dev\"");

  if (!dev) {
    write(FD_ERROR, "dev\n", SIZE_ERROR_MSG);
    return EXIT_ERROR;
  }

  dev += strlen("\"dev\"");

  char *first_quote = strchr(dev, '"');

  if (!first_quote) {
    write(FD_ERROR, "first_quote\n", SIZE_ERROR_MSG);
    return EXIT_ERROR;
  }

  char *second_quote = strchr(first_quote + 1, '"');

  if (!second_quote) {
    write(FD_ERROR, "second_quote\n", SIZE_ERROR_MSG);
    return EXIT_ERROR;
  }

  int len_id = second_quote - (first_quote + 1);

  if (len_id >= SIZE_NAME_IFACE)
    len_id = SIZE_NAME_IFACE - 1;

  memcpy(device_id, first_quote + 1, len_id);

  device_id[len_id] = '\0';

  char *val_str = strstr(str, "\"val\"");
  char temperature[SIZE_STR_TEMPERATURE];
  int index = 0;

  if (!val_str) {
    write(FD_ERROR, "val\n", SIZE_ERROR_MSG);
    return EXIT_ERROR;
  }

  val_str += strlen("val\n");

  while (*val_str && !isdigit(*val_str))
    val_str++;

  while (*val_str && (isdigit(*val_str) || *val_str == '.')) {
    if (index < SIZE_STR_TEMPERATURE - 1)
      temperature[index++] = *val_str;
    val_str++;
  }

  temperature[index] = '\0';

  int point_flag = -1, int_part = 0, frac_part = 0, div = 1;

  index = 0;

  if (temperature[0] == '-') {
    write(FD_ERROR, "sign '-'\n", SIZE_ERROR_MSG);
    return EXIT_ERROR;
  }

  for (; temperature[index]; index++) {
    if (temperature[index] == '.') {
      point_flag = 1;
      continue;
    }

    if (point_flag == -1) {
      int_part = int_part * DIGIT_NUMBER + (temperature[index] - '0');
    } else {
      frac_part = frac_part * DIGIT_NUMBER + (temperature[index] - '0');
      div *= DIGIT_NUMBER;
    }
  }

  *val = int_part + (float)(frac_part / div);

  char *seq_start = strstr(str, "\"seq\":");
  if (!seq_start) {
      write(FD_ERROR, "seq field not found\n", SIZE_ERROR_MSG);
      return EXIT_ERROR;
  }
  seq_start += 6;
  *seq = 0;
  while (*seq_start && isdigit(*seq_start)) {
      *seq = *seq * 10 + (*seq_start - '0');
      seq_start++;
  }

  return EXIT_SUCCESS;
}

char *command_to_string(int command) {
  switch (command) {
  case ON_LIGHT:
    return "on_light";
  case OFF_LIGHT:
    return "off_light";
  case ON_ALL:
    return "all_on";
  case OFF_ALL:
    return "all_off";
  default:
    return "unknown";
  }
}

int create_command_controller(char *payload, int id, int command) {
  const char *command_str = command_to_string(command);
  const char *middle = ", \"id\": ";
  const char *suffix = "\"}";
  char id_str[SIZE_STR_ID];

  if (!command_str || strcmp(command_str, "unknown") == 0)
    return -1;

  payload[0] = '\0';

  if (strlen(command_str) + strlen("{\"cmd\": \"") >= SIZE_PAYLOAD)
    return -1;

  strcat(payload, "{\"cmd\": \"");
  strcat(payload, command_str);
  strcat(payload, "\"");

  if (strlen(payload) + strlen(middle) >= SIZE_PAYLOAD)
    return EXIT_ERROR;

  strcat(payload, middle);

  int len_id = 0, temp_id = id;

  if (temp_id == 0) {
    id_str[len_id++] = '0';
  } else {
    while (temp_id && len_id < (int)(sizeof(id_str) - 1)) {
      id_str[len_id++] = '0' + (temp_id % DIGIT_NUMBER);
      temp_id /= DIGIT_NUMBER;
    }

    for (int i = 0; i < len_id / 2; i++) {
      char tmp = id_str[i];
      id_str[i] = id_str[len_id - 1 - i];
      id_str[len_id - 1 - i] = tmp;
    }
  }

  id_str[len_id] = '\0';

  if (strlen(payload) + strlen(id_str) >= SIZE_PAYLOAD)
    return EXIT_ERROR;

  strcat(payload, id_str);
  strcat(payload, suffix);

  return EXIT_SUCCESS;
}

void log_file_msg(char *msg, char *name_file) {
  int fd = open(name_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
  char newline = '\n';

  if (fd < 0)
    return;

  write(fd, msg, strlen(msg));
  write(fd, &newline, 1);
  close(fd);
}