#ifndef _STR_UTILS_H
#define _STR_UTILS_H

#include <stdarg.h>
#include <stddef.h>

void _reverse(char *, size_t);
unsigned int _atoui(const char *, int);
float _atof(const char *);
int _isdelim(char);
void _ftoa(float, char *, int);
void _uitoa(unsigned int, char *, int);
void binary(unsigned int, char *, size_t);

int sscanj(const char *, const char *, ...);
int snprintj(char *, size_t, const char *, ...);

#endif
