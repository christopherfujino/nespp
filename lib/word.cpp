#include "../include/word.h"

namespace NESPP {

Word::Word(uint8_t high, uint8_t low) {
  this->low = low;
  this->high = high;
}

Word Word::from16(uint16_t raw) {
  uint8_t low = 0xFF & raw;
  uint8_t high = (0xFF00 & raw) >> 8;
  return Word{high, low};
}

Word::Word() {
  low = 0;
  high = 0;
}

uint16_t Word::to16() { return low | (high << 8); }

} // namespace NESPP
