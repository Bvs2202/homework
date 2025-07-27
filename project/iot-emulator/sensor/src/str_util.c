#include "../../common/smart_home.h"
#include "../../common/str_utils.h"

void _reverse(char *s, size_t len) {
  for (size_t i = 0; i < len / 2; i++) {
    char tmp = s[i];
    s[i] = s[len - i - 1];
    s[len - i - 1] = tmp;
  }
}

unsigned int _atoui(const char *s, int base) {
  unsigned int res = 0;
  for (; *s; s++) {
    int digit;
    if (isdigit(*s))
      digit = *s - '0';
    else if (*s >= 'a' && *s <= 'f')
      digit = 10 + (*s - 'a');
    else if (*s >= 'A' && *s <= 'F')
      digit = 10 + (*s - 'A');
    else
      break;
    res = res * base + digit;
  }
  return res;
}

float _atof(const char *s) {
  float ipart = 0.0, frac_part = 0.0;

  while (isdigit(*s)) ipart = ipart * 10 + (*s++ - '0');

  if (*s == '.') {
    s++;
    float divisor = 1.0;
    while (isdigit(*s)) {
      frac_part = frac_part * 10 + (*s++ - '0');
      divisor *= 10;
    }
    frac_part /= divisor;
  }

  return ipart + frac_part;
}

int _isdelim(char c) {
  const char *delims = " \t\n\r\",:{}[]";
  for (const char *d = delims; *d; d++) {
    if (c == *d) return 1;
  }
  return 0;
}

void _ftoa(float val, char *buf, int prec) {
  int ipart = (int)val;
  _uitoa(ipart, buf, 10);
  size_t len = strlen(buf);

  buf[len++] = '.';
  float fpart = (float)(val - ipart);
  for (int i = 0; i < prec; i++) {
    fpart *= 10;
    int digit = (int)fpart;
    buf[len++] = '0' + digit;
    fpart -= digit;
  }
  buf[len] = '\0';
}

void _uitoa(unsigned int val, char *buf, int base) {
  const char *digits = "0123456789ABCDEF";
  size_t i = 0;
  do {
    buf[i++] = digits[val % base];
    val /= base;
  } while (val > 0);
  buf[i] = '\0';
  _reverse(buf, i);
}

void binary(unsigned int val, char *buf, size_t size) {
  size_t idx = 0;
  int i = 0;

  for (; i < 32 && idx < size - 1; i++) {
    buf[idx++] = (val & 1) ? '1' : '0';
    val >>= 1;
  }

  _reverse(buf, idx);

  buf[idx] = '\0';
}
