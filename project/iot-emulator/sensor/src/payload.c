#include "../include/payload.h"

float generate_temperature(void)
{
	return 20.0 + ((rand() % 100)) * 0.1;
}

void time_to_str(time_t ts, char* ptr)
{
	for (int i = 9; i >= 0; i--)
	{
		ptr[i] = '0' + (ts % 10);
		ts /= 10;
	}
}

void float_to_str(float temp, char *ptr)
{
	int integer = (int)temp;
	int frac = (int)((temp - integer) * 10);

	*ptr++ = '0' + integer / 10;
	*ptr++ = '0' + integer % 10;
	*ptr++ = '.';
	*ptr++ = '0' + frac;
}

void build_payload(char *buff, const char *name_sensor)
{
	time_t ts = time(NULL);
	float temp = generate_temperature();
	char *ptr = buff;

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

	*ptr = '}';
	ptr++;
	*ptr = '\0';
}