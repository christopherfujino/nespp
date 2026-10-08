#include "word.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static size_t tests = 0;
static size_t passes = 0;
constexpr size_t string_size = 1024;
char string_buf[string_size];

void expect(bool e) {
  assert(e);
  passes++;
  tests++;
}

static void one() {
  Word word = wordOfU16(0x00FE);
  uint8_t operand = 0x03;
  Word sum = wordPlus(&word, operand);
  expect(wordTo16(&sum) == 0x0101);
}

int main() {
  one();
  printf("%ld of %ld tests passed.\n", tests, passes);
}
