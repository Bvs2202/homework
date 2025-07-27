#include "encrypt.h"

const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void encode_base64(unsigned char *payload, int len_payload) {
  int j = 0;
  unsigned char buff[SIZE_PAYLOAD] = {0};
  uint8_t *ptr = (uint8_t*)buff;
  uint32_t group_1 = 0, group_2 = 0, group_3 = 0;
  uint32_t triple = 0;

  memcpy(buff, payload, len_payload);

  for (int i = 0; i < len_payload;) {
    group_1 = i < len_payload ? ptr[i++] : 0;
    group_2 = i < len_payload ? ptr[i++] : 0;
    group_3 = i < len_payload ? ptr[i++] : 0;

    triple = (group_1 << 16) | (group_2 << 8) | group_3;

    payload[j++] = b64_table[(triple >> 18) & 0x3F];
    payload[j++] = b64_table[(triple >> 12) & 0x3F];
    payload[j++] = b64_table[(triple >> 6) & 0x3F];
    payload[j++] = b64_table[(triple) & 0x3F];
  }
  switch (len_payload % 3) {
  case 1:
    payload[j - 2] = '=';
    payload[j - 1] = '=';
    break;
  case 2:
    payload[j - 1] = '=';
    break;
  }

  payload[j] = '\0';
}

void decode_base64(unsigned char *payload, int len_payload) {
  unsigned char buff[SIZE_PAYLOAD] = {0};
  unsigned char reverse_64table[SIZE_REVERSE_TABLE] = {0};
  int new_size = (len_payload * 3) / 4;
  int j = 0;

  if (payload[len_payload - 1] == '=') new_size--;
  if (payload[len_payload - 2] == '=') new_size--;

  memcpy(buff, payload, len_payload);
  memset(payload, 0, SIZE_PAYLOAD);

  for (int i = 0; i < SIZE_BASE64; i++) {
    reverse_64table[(unsigned char)b64_table[i]] = i;
  }

  for (int i = 0; i < len_payload;) {
    uint32_t sextet_a = buff[i] == '=' ? 0 & i++ : reverse_64table[buff[i++]];
    uint32_t sextet_b = buff[i] == '=' ? 0 & i++ : reverse_64table[buff[i++]];
    uint32_t sextet_c = buff[i] == '=' ? 0 & i++ : reverse_64table[buff[i++]];
    uint32_t sextet_d = buff[i] == '=' ? 0 & i++ : reverse_64table[buff[i++]];

    uint32_t triple = (sextet_a << 18) + (sextet_b << 12) + (sextet_c << 6) + (sextet_d);

    if (j < new_size) {
      payload[j++] = (triple >> 16) & 0xFF;
    }
    if (j < new_size) {
      payload[j++] = (triple >> 8) & 0xFF;
    }
    if (j < new_size) {
      payload[j++] = (triple) & 0xFF;
    }
  }

  payload[j] = '\0';
}

void encrypt_XOR(unsigned char *payload, int len_payload) {
  const uint8_t key = 0xAA;
  for (int i = 0; i < len_payload; i++) {
    payload[i] = payload[i] ^ key;
  }
}

void decrypt_XOR(unsigned char *payload, int len_payload) {
  encrypt_XOR(payload, len_payload);
}

int decrypt_payload(unsigned char *payload) {
  int len_payload = strlen((const char*)payload);

  if (payload == NULL || len_payload <= 0) {
    return -1;
  }

  decode_base64(payload, len_payload);

  len_payload = strlen((const char*)payload);

  decrypt_XOR(payload, len_payload);

  return 0;
}

int encrypt_payload(unsigned char *payload) {
  int len_payload = strlen((const char*)payload);
  int len_b64 = (((len_payload + 2) / 3) * 4 + 1);

  if (payload == NULL || len_b64 > SIZE_PAYLOAD)
  {
    return -1;
  }

  encrypt_XOR(payload, len_payload);

  encode_base64(payload, len_payload);

  return 0;
}