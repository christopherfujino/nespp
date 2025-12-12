#pragma once

#include <cstdint>

namespace Address {

// TODO: rename this a Word
struct Absolute {
  /// $HHLL
  Absolute(uint16_t);
  Absolute(uint8_t high, uint8_t low);
  Absolute();

  uint8_t low;
  uint8_t high;

  uint16_t to16();
};

} // namespace Address
