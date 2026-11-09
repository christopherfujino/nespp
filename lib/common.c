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
    free((void *)result->err);
  }
}

Result vchrisprintf(char **strp, const char *fmt, va_list args) {
  constexpr size_t initialBuflen = 512;
  size_t currentBuflen = initialBuflen;
  static char staticBuf[initialBuflen];
  char *buf = staticBuf;
  int n = 0;

  while (1) {
    n = vsnprintf(buf, currentBuflen, fmt, args);
    // TODO validate n
    if (n < 0) {
      return resultErr("vsnprintf() failed");
    } else if (n == (int)currentBuflen) {
      currentBuflen *= 2;
      if (buf == staticBuf) {
        buf = malloc(currentBuflen);
      } else {
        buf = realloc(buf, currentBuflen);
      }
      if (buf == nullptr) {
        return resultErr("failed to allocate memory");
      }
    } else {
      // success
      break;
    }
  }

  *strp = malloc(n + 1);
  strncpy(*strp, buf, n);
  (*strp)[n] = '\0';
  if (buf != staticBuf) {
    free(buf);
  }
  return resultOk();
}

Result chrisprintf(char **restrict strp, const char *restrict fmt, ...) {
  va_list args = {};
  va_start(args);
  Result result = vchrisprintf(strp, fmt, args);
  va_end(args);

  return result;
}

Result resultErr(const char *fmt, ...) {
  char *msg = nullptr;
  va_list args = {};
  {
    va_start(args, fmt);
    resultTry(vchrisprintf(&msg, fmt, args));
    va_end(args);
  }

  return WRAP_LITERAL(Result){
      .tag = resultErr_t,
      .err = msg,
  };
}
