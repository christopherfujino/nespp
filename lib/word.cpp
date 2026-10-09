#include "../include/word.h"

extern "C" {

uint16_t wordTo16(Word *word) { return word->low | (word->high << 8); }

Word wordPlus(Word *self, int other) {
  uint16_t u16 = ((static_cast<uint16_t>(self->high) << 8) | self->low) + other;
  return Word{
      .high = static_cast<uint8_t>((0xFF00 & u16) >> 8),
      .low = static_cast<uint8_t>(0xFF & u16),
  };
}

Word wordMinus(Word *self, int other) {
  uint16_t u16 = ((static_cast<uint16_t>(self->high) << 8) | self->low) - other;
  return Word{
      .high = static_cast<uint8_t>((0xFF00 & u16) >> 8),
      .low = static_cast<uint8_t>(0xFF & u16),
  };
}

Word wordOfU16(uint16_t raw) {
  uint8_t low = 0xFF & raw;
  uint8_t high = (0xFF00 & raw) >> 8;
  return Word{
      .high = high,
      .low = low,
  };
}

} // extern "C"
