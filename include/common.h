#ifndef __MONOREPO_SRC_NESPP_INCLUDE_COMMON_H
#define __MONOREPO_SRC_NESPP_INCLUDE_COMMON_H

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
  return Result{
      .tag = resultOk_t,
      .ok = nullptr,
  };
}

static inline Result resultErr(const char *msg) {
  return Result{
      .tag = resultErr_t,
      .err = msg,
  };
}

#endif // __MONOREPO_SRC_NESPP_INCLUDE_COMMON_H
