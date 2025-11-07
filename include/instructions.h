#pragma once

#include <cstdint>
#include <format>
#include <utility>
#include <vector>

namespace Instructions {

enum OpCode : uint8_t {
  JMP_ABS = 0x4C,
  LDA_ABS = 0xAD,
  STA_ABS = 0x8D,
  LDA_IMM = 0xA9,
  LDX_IMM = 0xA2,
  BPL_REL = 0x10,
  RTS = 0x60, // return from subroutine
  SEI = 0x78, // set interrupt disable
  CLD = 0xD8, // Clear decimal
  TXS = 0x9A, // Transfer index X to stack register
};

struct AbsoluteAddress {
  uint8_t low;
  uint8_t high;
};

struct Instruction {
  OpCode opCode;
  union {
    AbsoluteAddress absolute;
    uint8_t immediate;
    void *implied;
    uint8_t relative;
  } operand;
};

std::vector<Instruction> deserialize(uint8_t *data, int len);

std::pair<Instruction, int> decodeInstruction(uint8_t *src, int idx);

} // namespace Instructions
