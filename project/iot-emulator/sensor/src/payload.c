#include "../include/payload.h"

float generate_temperature(void) { return 20.0 + ((rand() % 100)) * 0.1; }

void time_to_str(time_t ts, unsigned char *ptr) {
  for (int i = 9; i >= 0; i--) {
    ptr[i] = '0' + (ts % 10);
    ts /= 10;
  }
}

void float_to_str(float temp, unsigned char *ptr) {
  int integer = (int)temp;
  int frac = (int)((temp - integer) * 10);

  *ptr++ = '0' + integer / 10;
  *ptr++ = '0' + integer % 10;
  *ptr++ = '.';
  *ptr++ = '0' + frac;
}

void packet_counter_str(int packet_counter, unsigned char *ptr) {
  if (packet_counter == 0) {
    *ptr++ = '0';
    return;
  }

  int digits = 0;
  int temp = packet_counter;
  while (temp != 0) {
    digits++;
    temp /= 10;
  }

  temp = packet_counter;
  for (int i = digits - 1; i >= 0; i--) {
    ptr[i] = '0' + (temp % 10);
    temp /= 10;
  }
}

void build_payload(unsigned char *buff, const char *name_sensor, int packet_counter) {
  time_t ts = time(NULL);
  float temp = generate_temperature();
  unsigned char *ptr = buff;

  *ptr = '{';
  ptr++;

  memcpy(ptr, "\"dev\":\"", strlen("\"dev\":\""));
  ptr += strlen("\"dev\":\"");

  memcpy(ptr, name_sensor, strlen(name_sensor));
  ptr += strlen(name_sensor);
  *ptr = '\"';
  ptr++;

  memcpy(ptr, ", \"val\":", strlen(", \"val\":"));
  ptr += strlen(", \"val\":");
  float_to_str(temp, ptr);
  ptr += SIZE_TEMPERATURE;

  memcpy(ptr, ", \"ts\":", strlen(", \"ts\":"));
  ptr += strlen(", \"ts\":");
  time_to_str(ts, ptr);
  ptr += SIZE_TS;

  memcpy(ptr, ",\"seq\":", 7);
  ptr += 7;
  packet_counter_str(packet_counter, ptr);

  int digits = 0;
  int temp_counter = packet_counter;
  do {
    digits++;
    temp_counter /= 10;
  } while (temp_counter != 0);
  
  ptr += digits;

  *ptr = '}';
  ptr++;
  *ptr = '\0';
}