#include "word.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> // malloc()

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

constexpr size_t string_size = 1024;
char string_buf[string_size];

static Suite SuiteNew() {
  constexpr size_t init_cap = 8;
  SuiteEntry *buf = malloc(sizeof(SuiteEntry) * init_cap);
  return (Suite){
      .tests = 0,
      .test_cap = init_cap,
      .test_buf = buf,
  };
}

static void mktest(Suite *suite, const char *name, Test test) {
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

static void runsuite(Suite *suite) {
  size_t passes = 0;
  for (size_t i = 0; i < suite->tests; i++) {
    bool result = suite->test_buf[i].test();
    if (result) {
      passes++;
    } else {
      printf("Test \"%s\" failed.\n", suite->test_buf[i].name);
    }
  }
  printf("%ld of %ld tests passed.\n", passes, suite->tests);
}

static bool test0() {
  Word word = wordOfU16(0x00FE);
  uint8_t operand = 0x03;
  Word sum = wordPlus(&word, operand);
  return wordTo16(&sum) == 0x0101;
}

int main() {
  Suite suite = SuiteNew();
  mktest(&suite, "foo", test0);

  runsuite(&suite);
}
