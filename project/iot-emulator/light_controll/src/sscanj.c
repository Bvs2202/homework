#include "../../common/smart_home.h"
#include "../include/str_utils.h"

int sscanj(const unsigned char *str, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  int count = 0;
  size_t str_idx = 0;
  size_t fmt_idx = 0;

  while (fmt[fmt_idx] && str[str_idx]) {
    if (isspace(fmt[fmt_idx])) {
      while (isspace(fmt[fmt_idx])) fmt_idx++;
      while (isspace(str[str_idx])) str_idx++;
      continue;
    }

    if (fmt[fmt_idx] == 'h' && fmt[fmt_idx + 1] == 'h' &&
        fmt[fmt_idx + 2] == 'x') {
      fmt_idx += 3;
      unsigned char *x_ptr = va_arg(args, unsigned char *);
      *x_ptr = (unsigned char)_atoui((const char*)(str + str_idx), 16);
      count++;

      while (isxdigit(str[str_idx])) str_idx++;
      if (fmt[fmt_idx] == ':' && str[str_idx] == ':') {
        str_idx++;
        fmt_idx++;
      }
      continue;
    }

    if (fmt[fmt_idx] == '%') {
      fmt_idx++;
      if (!fmt[fmt_idx]) break;

      while (isspace(str[str_idx])) str_idx++;
      if (!str[str_idx]) break;

      if (fmt[fmt_idx] == 'h' && fmt[fmt_idx + 1] == 'h' &&
          fmt[fmt_idx + 2] == 'x') {
        fmt_idx += 3;
        unsigned char *x_ptr = va_arg(args, unsigned char *);
        *x_ptr = (unsigned char)_atoui((const char*)(str + str_idx), 16);
        count++;

        while (isxdigit(str[str_idx])) str_idx++;
        if (fmt[fmt_idx] == ':' && str[str_idx] == ':') {
          str_idx++;
          fmt_idx++;
        }
        continue;
      }

      switch (fmt[fmt_idx]) {
        case 'd': {
          int *d_ptr = va_arg(args, int *);
          *d_ptr = (int)_atoui((const char*)(str + str_idx), 10);
          count++;
          while (isdigit(str[str_idx])) str_idx++;
          break;
        }
        case 'x': {
          unsigned int *x_ptr = va_arg(args, unsigned int *);
          *x_ptr = _atoui((const char*)str + str_idx, 16);
          count++;
          while (isxdigit(str[str_idx])) str_idx++;
          break;
        }
        case 'f': {
          float *f_ptr = va_arg(args, float *);
          *f_ptr = _atof((const char*)str + str_idx);
          count++;
          while (isdigit(str[str_idx]) || str[str_idx] == '.') str_idx++;
          break;
        }
        case '[': {
          if (fmt[fmt_idx + 1] == '^' && fmt[fmt_idx + 2] == '\"') {
            fmt_idx += 3;
            char *s_ptr = va_arg(args, char *);
            size_t cnt = 0;
            while (str[str_idx] && str[str_idx] != '\"') {
              s_ptr[cnt++] = str[str_idx++];
            }
            s_ptr[cnt] = '\0';
            count++;
          }
          break;
        }
        case 's': {
          char *s_ptr = va_arg(args, char *);
          size_t cnt = 0;
          while (!_isdelim(str[str_idx]) && str[str_idx]) {
            s_ptr[cnt++] = str[str_idx++];
          }
          s_ptr[cnt] = '\0';
          count++;
          break;
        }
      }
      fmt_idx++;
    }
    if (str[str_idx] != fmt[fmt_idx]) break;

    str_idx++;
    fmt_idx++;
  }

  va_end(args);
  return count;
}
