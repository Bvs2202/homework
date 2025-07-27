#ifndef PAYLOAD_H
#define PAYLOAD_H

#include "../../common/smart_home.h"

#define SIZE_TEMPERATURE 4
#define SIZE_TS 10

float generate_temparature(void);
void time_to_str(time_t ts, unsigned char *ptr);
void float_to_str(float temp, unsigned char *ptr);
void build_payload(unsigned char *buff, const char *name_sensor, int packet_counter);

#endif
