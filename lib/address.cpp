#include "../include/address.h"

namespace Address {

Absolute::Absolute(uint8_t high, uint8_t low) {
  this->low = low;
  this->high = high;
}

Absolute::Absolute(uint16_t raw) {
  low = 0xFF & raw;
  high = (0xFF00 & raw) >> 8;
}

Absolute::Absolute() {
  low = 0;
  high = 0;
}

uint16_t Absolute::to16() { return low | (high << 8); }

} // namespace Address
