#ifndef _LAMP_H
#define _LAMP_H

#include <stdint.h>

void turn_on_light(uint32_t, uint32_t *);
void turn_off_light(uint32_t, uint32_t *);
void turn_on_all(uint32_t *);
void turn_off_all(uint32_t *);

#endif
