#ifndef __MONOREPO_SRC_NESPP_INCLUDE_COMMON_H
#define __MONOREPO_SRC_NESPP_INCLUDE_COMMON_H

#include <stdarg.h> // va_list
#include <stdio.h>  // vfprintf()
#include <stdlib.h> // abort(), exit()

#ifdef __cplusplus
extern "C" {
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
  bool ownsString;
  char *s;
} WrappedString;

typedef struct {
  enum ResultTag tag;
  union {
    const void *ok;
    WrappedString err;
  };
} Result;

void resultDispose(Result *result);

static inline Result resultOk() {
  return WRAP_LITERAL(Result){
      .tag = resultOk_t,
      .ok = nullptr,
  };
}

static inline Result resultErr(char *msg, bool ownsString) {
  return WRAP_LITERAL(Result){
      .tag = resultErr_t,
      .err =
          {
              .ownsString = ownsString,
              .s = msg,
          },
  };
}

[[noreturn]] [[gnu::format(printf, 1, 2)]]
static inline void panic(const char *fmt, ...) {
  // TODO: streamline this when we universally use C23
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
#ifdef NDEBUG
  exit(1);
#else
  abort();
#endif
}

// basically `man 3 asprintf`
// TODO: use `restrict` when in C world
[[gnu::format(printf, 2, 3)]]
void chrisprintf(char **strp, const char *fmt, ...);

#ifdef __cplusplus
} // extern "C" {
#endif
#endif // __MONOREPO_SRC_NESPP_INCLUDE_COMMON_H
