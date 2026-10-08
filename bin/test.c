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

int main() {
  Word word = wordOfU16(0xC00E);
  uint8_t operand = 0xFB;
  auto sum = wordPlus(&word, operand);
  printf("%04X\n", wordTo16(&sum));

  printf("%ld of %ld tests passed.\n", tests, passes);
}
