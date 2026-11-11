#ifndef __MONOREPO_SRC_NESPP_INCLUDE_COMMON_H
#define __MONOREPO_SRC_NESPP_INCLUDE_COMMON_H

#include <stdarg.h> // va_list
#include <stdio.h>  // vfprintf()
#include <stdlib.h> // abort(), exit()

// TODO: deprecate
#ifdef __cplusplus
extern "C" {
#define WRAP_LITERAL(v) v
#else
#define WRAP_LITERAL(v) (v)
#endif

// TODO: depend on //src/chrislib/chrislib.h when in C world
[[noreturn]] [[gnu::format(printf, 1, 2)]]
static inline void panic(const char *fmt, ...) {
  // TODO: streamline this when we universally use C23
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  fflush(stderr);
  va_end(args);
#ifdef NDEBUG
  exit(1);
#else
  abort();
#endif
}

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

void resultDispose(Result *result);

static inline Result resultOk() {
  return WRAP_LITERAL(Result){
      .tag = resultOk_t,
      .ok = nullptr,
  };
}

// Ensure it is ok
static inline void resultTry(Result result) {
  switch (result.tag) {
  case resultOk_t:
    return;
  case resultErr_t:
    panic("%s", result.err);
    break;
  }
}

[[gnu::format(printf, 1, 2)]] [[nodiscard]]
Result resultErr(const char *fmt, ...);

// basically `man 3 asprintf`
// TODO: use `restrict` when in C world
[[gnu::format(printf, 2, 3)]] [[nodiscard]]
Result chrisprintf(char **strp, const char *fmt, ...);

// TODO: use `restrict` when in C world
[[nodiscard]]
Result vchrisprintf(char **strp, const char *fmt, va_list args);

static inline void chrisFreeString(char **pp) {
  if (*pp == nullptr) {
    return;
  }
  free(*pp);
  *pp = nullptr;
}

#ifdef __cplusplus
} // extern "C" {
#endif // __cplusplus

#endif // __MONOREPO_SRC_NESPP_INCLUDE_COMMON_H
