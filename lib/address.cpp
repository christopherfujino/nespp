#include "../include/address.h"

namespace Address {

uint16_t Absolute::to16() { return low | (high << 8); }

} // namespace Address
