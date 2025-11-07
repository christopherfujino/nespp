#include <cstring>

#include "../include/instructions.h"

namespace Instructions {

std::vector<OpCode> deserialize(uint8_t *src, int len) {
  int index = 0;
  uint8_t *dataPtr = src;
  std::vector<OpCode> opcodes;

  decodeInstruction(dataPtr + index);
  // TODO: this will copy
  return opcodes;
}

std::pair<Instruction, int> decodeInstruction(uint8_t *src) {
  switch (*src) {
  case 0x4c:
    return std::make_pair(
        Instruction{
            .opCode = OpCode::JMP_ABS,
            .operand = {.absolute = AbsoluteAddress{.low = *(src + 1),
                                                    .high = *(src + 2)}}},
        3);
  default:
    // This is never freed
    char *msg = new char[256];
    snprintf(msg, 256, "Unimplmented instruction 0x%X", *src);
    throw msg;
  }
}

} // namespace Instructions
