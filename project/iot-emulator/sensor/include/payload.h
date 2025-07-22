#ifndef PAYLOAD_H
#define PAYLOAD_H

#include "../include/sensor_protocol.h"

#define SIZE_TEMPERATURE 4
#define SIZE_TS 10

float generate_temparature(void);
void time_to_str(time_t ts, char* ptr);
void float_to_str(float temp, char *ptr);
void build_payload(char *buff, const char *name_sensor);

#endif
