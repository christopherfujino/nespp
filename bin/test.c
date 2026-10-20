#include <stdint.h> // uint8_t
#include <stdlib.h> // malloc()
#include <string.h> // memset()

#include "common.h"
#include "test.h"
#include "word.h"

constexpr size_t string_size = 1024;
char string_buf[string_size];

static char *mkstring(size_t len) {
  char *s = malloc(len);

  memset(s, 'c', len - 1);
  s[len - 1] = '\0';

  return s;
}

static bool test0() {
  Word word = wordOfU16(0x00FE);
  uint8_t operand = 0x03;
  Word sum = wordPlus(&word, operand);
  return wordTo16(&sum) == 0x0101;
}

static bool test1() {
  constexpr size_t buflen = 1024;
  char *string = mkstring(buflen + 1);
  char *newString = nullptr;
  chrisprintf(&newString, "%s", string);
  char *cursor = newString;
  size_t count = 0;
  while (*cursor != '\0') {
    if (*cursor != 'c') {
      abort();
    }
  }
  return true;
}

int main() {
  Suite suite = SuiteNew();
  suiteAddTest(&suite, "foo", test0);
  suiteAddTest(&suite, "common:chrisprintf", test1);

  suiteRun(&suite);
}
