#ifndef __MONOREPO_SRC_NESPP_INCLUDE_COMMON_H
#define __MONOREPO_SRC_NESPP_INCLUDE_COMMON_H

#include <stdarg.h> // va_list
#include <stdio.h>  // vfprintf()
#include <stdlib.h> // abort()

#ifdef __cplusplus
#define WRAP_LITERAL(v) v
#else
#define WRAP_LITERAL(v) (v)
#endif

// TODO: inline this when in C world
enum ResultTag {
  resultOk_t,
  resultErr_t,
};

typedef struct {
  enum ResultTag tag;
  union {
    const void *ok;
    const char *err;
  };
} Result;

static inline Result resultOk() {
  return WRAP_LITERAL(Result){
      .tag = resultOk_t,
      .ok = nullptr,
  };
}

static inline Result resultErr(const char *msg) {
  return WRAP_LITERAL(Result){
      .tag = resultErr_t,
      .err = msg,
  };
}

[[noreturn]] [[gnu::format(printf, 1, 2)]]
static inline void panic(const char *fmt, ...) {
  // TODO: do something different in RELEASE?
  // TODO: streamline this when we universally use C23
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  abort();
}

#endif // __MONOREPO_SRC_NESPP_INCLUDE_COMMON_H
