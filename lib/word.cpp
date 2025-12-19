#include "../include/word.h"

namespace NESPP {

Word::Word(uint8_t high, uint8_t low) {
  this->low = low;
  this->high = high;
}

Word::Word(uint16_t raw) {
  low = 0xFF & raw;
  high = (0xFF00 & raw) >> 8;
}

Word::Word() : low(0), high(0) {}

uint16_t Word::to16() { return low | (high << 8); }

Word Word::operator+(int other) {
  return Word(((static_cast<uint16_t>(high) << 8) | low) + other);
}

Word Word::operator-(int other) {
  return Word(((static_cast<uint16_t>(high) << 8) | low) - other);
}

void Word::operator+=(int other) {
  auto newThis = *this + other;
  *this = newThis;
}

} // namespace NESPP
