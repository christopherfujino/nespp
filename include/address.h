#pragma once

#include <cassert>
#include <cstdint>

namespace Address {

// TODO: rename this a Word
// TODO: make this a wrapper around a uint16_t?
class Absolute {
public:
  /// $HHLL
  Absolute(uint16_t);
  Absolute(uint8_t high, uint8_t low);
  Absolute();

  uint8_t low;
  uint8_t high;

  uint16_t to16();

  Absolute operator+(int other) {
    int sum = other + low;
    if (sum > 0xFF) {
      uint8_t _low = 0xFF & sum;
      uint16_t _high = ((0xFF00 & sum) >> 8) + high;
      assert(_high <= 0xFF);
      return Absolute{(uint8_t)_high, _low};
    } else {
      // no overflow
      return Absolute{high, (uint8_t)sum};
    }
  }

  void operator+=(int other) {
    auto newThis = *this + other;
    this->low = newThis.low;
    this->high = newThis.high;
  }
};

} // namespace Address
