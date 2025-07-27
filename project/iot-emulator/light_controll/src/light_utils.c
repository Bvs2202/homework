#include "../include/lamp.h"

void turn_on_light(uint32_t light_id, uint32_t *mask) {
  if (light_id < 1 || light_id > 32) return;
  *mask |= (1U << (light_id - 1));
  ;
}

void turn_off_light(uint32_t light_id, uint32_t *mask) {
  if (light_id < 1 || light_id > 32) return;
  *mask &= ~(1U << (light_id - 1));
  ;
}

void turn_on_all(uint32_t *mask) { *mask |= ~0U; }

void turn_off_all(uint32_t *mask) { *mask = 0; }
