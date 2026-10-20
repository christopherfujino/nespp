#include <stdarg.h> // va_end(), va_list, va_start()
#include <stdint.h> // uint8_t
#include <stdio.h>  // snprintf()
#include <stdlib.h> // malloc(), free()
#include <string.h> // strncpy()

#include "common.h"

void resultDispose(Result *result) {
  switch (result->tag) {
  case resultOk_t:
    return;
  case resultErr_t:
    if (result->err.ownsString) {
      free(result->err.s);
    }
  }
}

void chrisprintf(char **restrict strp, const char *restrict fmt, ...) {
  constexpr size_t initialBuflen = 1024;
  size_t currentBuflen = initialBuflen;
  char initialBuf[initialBuflen];
  char *buf = initialBuf;
  va_list args = {};
  int n = 0;
  va_start(args);
  while (1) {
    n = vsnprintf(buf, currentBuflen, fmt, args);
    // TODO validate n
    if (n < 0) {
      panic("vsnprintf() failed\n");
    } else if (n == (int)currentBuflen) {
      currentBuflen *= 2;
      if (buf == initialBuf) {
        buf = malloc(currentBuflen);
      } else {
        buf = realloc(buf, currentBuflen);
      }
      if (buf == nullptr) {
        panic("failed to allocate memory");
      }
    } else {
      // success
      break;
    }
  }
  va_end(args);

  *strp = malloc(n + 1);
  strncpy(*strp, buf, n);
  (*strp)[n] = '\0';
  if (buf != initialBuf) {
    free(buf);
  }
}
