#include "../../common/smart_home.h"
#include "../../common/str_utils.h"

#define BINARY_SIZE 34

int snprintj(char *str, size_t size, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  size_t idx = 0;

  for (int i = 0; fmt[i] != '\0' && idx < size; i++) {
    if (fmt[i] == '%') {
      i++;
      while (fmt[i] && !isalpha(fmt[i])) i++;
      if (!fmt[i]) break;

      char buffer[34];
      const char *s_arg = NULL;

      switch (fmt[i]) {
        case 'd': {
          int d_val = va_arg(args, int);
          _uitoa((unsigned int)d_val, buffer, 10);
          s_arg = buffer;
          break;
        }

        case 'x': {
          unsigned int x_val = va_arg(args, unsigned int);
          _uitoa(x_val, buffer, 16);
          s_arg = buffer;
          break;
        }

        case 'f': {
          double f_val = va_arg(args, double);
          _ftoa((float)f_val, buffer, 3);
          s_arg = buffer;
          break;
        }

        case 's': {
          s_arg = va_arg(args, const char *);
          if (!s_arg) s_arg = "(null)";
          break;
        }

        default: {
          buffer[0] = fmt[i];
          buffer[1] = '\0';
          s_arg = buffer;
          break;
        }
      }

      if (s_arg) {
        for (int j = 0; s_arg[j]; j++) {
          if (idx >= size - 1) break;
          str[idx++] = s_arg[j];
        }
      }
    } else {
      if (idx >= size - 1) break;
      str[idx++] = fmt[i];
    }
  }

  if (size > 0) str[idx] = '\0';
  va_end(args);
  return idx;
}
