#ifndef __MONOREPO_SRC_NESPP_INCLUDE_WORD_H
#define __MONOREPO_SRC_NESPP_INCLUDE_WORD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// TODO: make this a wrapper around a uint16_t?
typedef struct Word {
  uint8_t high;
  uint8_t low;
} Word;

uint16_t wordTo16(Word *);

Word wordPlus(Word *self, int other);
Word wordMinus(Word *self, int other);

Word wordOfU16(uint16_t raw);

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // __MONOREPO_SRC_NESPP_INCLUDE_WORD_H
