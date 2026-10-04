#include "../include/word.h"

uint16_t wordTo16(Word *word) { return word->low | (word->high << 8); }

Word wordPlus(Word *self, int other) {
  uint16_t u16 = ((static_cast<uint16_t>(self->high) << 8) | self->low) + other;
  return Word{
      .low = static_cast<uint8_t>(0xFF & u16),
      .high = static_cast<uint8_t>((0xFF00 & u16) >> 8),
  };
}

Word wordMinus(Word *self, int other) {
  uint16_t u16 = ((static_cast<uint16_t>(self->high) << 8) | self->low) - other;
  return Word{
      .low = static_cast<uint8_t>(0xFF & u16),
      .high = static_cast<uint8_t>((0xFF00 & u16) >> 8),
  };
}
