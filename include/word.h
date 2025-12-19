#pragma once

#include <cstdint>

namespace NESPP {

// TODO: make this a wrapper around a uint16_t?
class Word {
public:
  Word(uint8_t high, uint8_t low);
  Word();
  /// $HHLL
  explicit Word(uint16_t);

  uint8_t low;
  uint8_t high;

  uint16_t to16();

  Word operator+(int other);
  Word operator-(int other);

  void operator+=(int other);
};

} // namespace NESPP
