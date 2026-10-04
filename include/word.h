#pragma once

#include <stdint.h>

// TODO: make this a wrapper around a uint16_t?
struct Word {
  uint8_t low;
  uint8_t high;
};

uint16_t wordTo16(Word *);

Word wordPlus(Word *self, int other);
Word wordMinus(Word *self, int other);

inline Word wordOfU16(uint16_t raw) {
  uint8_t low = 0xFF & raw;
  uint8_t high = (0xFF00 & raw) >> 8;
  return Word{.low = low, .high = high};
}
