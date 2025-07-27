#ifndef ENCRYPT_H
#define ENCRYPT_H

#include "../smart_home.h"

#define SIZE_REVERSE_TABLE 256
#define SIZE_BASE64        64

void encode_base64(unsigned char *payload, int len_payload);
void encrypt_XOR(unsigned char *payload, int len_payload);
void decode_base64(unsigned char *payload, int len_payload);
void decrypt_XOR(unsigned char *payload, int len_payload);
int decrypt_payload(unsigned char *payload);
int encrypt_payload(unsigned char *payload);

#endif
