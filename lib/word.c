#include "word.h"
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

uint16_t wordTo16(Word *word) { return word->low | (word->high << 8); }

Word wordPlus(Word *self, int other) {
  uint16_t u16 = (((uint16_t)(self->high) << 8) | self->low) + other;
  return WRAP_LITERAL(Word){
      .high = (uint8_t)((0xFF00 & u16) >> 8),
      .low = (uint8_t)(0xFF & u16),
  };
}

Word wordMinus(Word *self, int other) {
  uint16_t u16 = (((uint16_t)(self->high) << 8) | self->low) - other;
  return WRAP_LITERAL(Word){
      .high = (uint8_t)((0xFF00 & u16) >> 8),
      .low = (uint8_t)(0xFF & u16),
  };
}

Word wordOfU16(uint16_t raw) {
  uint8_t low = 0xFF & raw;
  uint8_t high = (0xFF00 & raw) >> 8;
  return WRAP_LITERAL(Word){
      .high = high,
      .low = low,
  };
}

#ifdef __cplusplus
} // extern "C" {
#endif
