#ifndef SERVER_PARSE_H
#define SERVER_PARSE_H

#include "server_config.h"
#include "smart_home.h"

#define LOG_FILE_NAME "data.log"
#define SIZE_WINDOW 1024
#define SRC_PORT 10000
#define DEST_PORT 7777
#define MAX_TEMPERATURE 28.0
#define MAX_CONTROLLERS 5
#define SIZE_STR_TEMPERATURE 16
#define DIGIT_NUMBER 10
#define SIZE_STR_ID 12
#define FD_ERROR 2
#define FD_OUT 1
#define SIZE_ERROR_MSG 30
#define RAND_ID 4
#define RAND_CMD 2
#define EXIT_ERROR -1

void log_file_msg(char *msg, char *name_file);
struct Controller *get_controller(const char *device_id,
                                  struct Controller *controllers,
                                  int *count_device);
char *command_to_string(int command);
int parse_sensor(const char *str, char *device_id, float *value, int *seq);
int create_command_controller(char *payload, int id, int command);

#endif