#pragma once

#include <cstdint>
#include <format>
#include <utility>
#include <vector>

namespace Instructions {

enum class OpCode {
  JMP_ABS,
};

struct AbsoluteAddress {
  uint8_t low;
  uint8_t high;
};

struct Instruction {
  OpCode opCode;
  union {
    AbsoluteAddress absolute;
  } operand;
};

std::vector<OpCode> deserialize(uint8_t *data, int len);

std::pair<Instruction, int> decodeInstruction(uint8_t *src);

} // namespace Instructions
