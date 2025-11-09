#pragma once

#include <cstdint>

namespace Address {

// TODO remove
struct Absolute {
  uint8_t low;
  uint8_t high;

  uint16_t to16();
};

} // namespace Address
