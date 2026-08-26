#pragma once

#include <cstdint>

// TODO: make this a wrapper around a uint16_t?
struct Word {
  Word(uint8_t high, uint8_t low);
  explicit Word();
  /// $HHLL
  explicit Word(uint16_t);

  uint8_t low;
  uint8_t high;

  uint16_t to16();

  Word operator+(int other);
  Word operator-(int other);

  void operator+=(int other);
};
