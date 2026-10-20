#ifndef __MONOREPO_SRC_NESPP_INCLUDE_TEST_H
#define __MONOREPO_SRC_NESPP_INCLUDE_TEST_H

#include <stddef.h> // size_t
#include <stdio.h>  // fprintf(), stderr
#include <stdlib.h> // malloc()

#ifdef __cplusplus
#error "C++ is not yet supported"
#endif

typedef bool (*Test)();
typedef struct {
  const char *name;
  Test test;
} SuiteEntry;

typedef struct Suite {
  size_t tests;
  size_t test_cap;
  SuiteEntry *test_buf;
} Suite;

static inline Suite SuiteNew() {
  constexpr size_t init_cap = 8;
  SuiteEntry *buf = malloc(sizeof(SuiteEntry) * init_cap);
  return (Suite){
      .tests = 0,
      .test_cap = init_cap,
      .test_buf = buf,
  };
}

static inline void suiteAddTest(Suite *suite, const char *name, Test test) {
  if (suite->tests == suite->test_cap) {
    suite->test_cap *= 2;
    suite->test_buf = realloc(suite->test_buf, suite->test_cap);
    if (suite->test_buf == nullptr) {
      abort();
    }
  }
  suite->test_buf[suite->tests] = (SuiteEntry){
      .name = name,
      .test = test,
  };
  suite->tests++;
}

static inline void suiteRun(Suite *suite) {
  size_t passes = 0;
  for (size_t i = 0; i < suite->tests; i++) {
    bool result = suite->test_buf[i].test();
    if (result) {
      passes++;
    } else {
      fprintf(stderr, "Test \"%s\" failed.\n", suite->test_buf[i].name);
    }
  }
  printf("%ld of %ld tests passed.\n", passes, suite->tests);
}

static inline void expect(bool condition) {
  if (!condition) {
    abort();
  }
}

#endif // __MONOREPO_SRC_NESPP_INCLUDE_TEST_H
