#pragma once

#include <cstdint>
#include <format>
#include <utility>
#include <vector>

namespace Instructions {

enum OpCode : uint8_t {
  JMP_ABS = 0x4C,
  JSR_ABS = 0x20,
  LDA_ABS = 0xAD,
  STA_ABS = 0x8D,
  STX_ABS = 0x8E,
  STY_ABS = 0x8C,
  LDA_ABS_X = 0xBD,
  CPX_IMM = 0xE0, // Compare X with memory (X - M)
  LDA_IMM = 0xA9,
  LDX_IMM = 0xA2,
  LDY_IMM = 0xA0,
  BPL_REL = 0x10,
  BNE_REL = 0xD0, // Branch on not equal to zero
  LDA_ZERO = 0xA5,
  STA_ZERO = 0x85,
  CLD = 0xD8, // Clear decimal
  DEX = 0xCA, // Decrement X by one
  DEY = 0x88, // Decrement Y by one
  INX = 0xE8, // Increment X by one
  INY = 0xC8, // Increment Y by one
  RTS = 0x60, // return from subroutine
  SEI = 0x78, // set interrupt disable
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
    uint8_t zeropage;
  } operand;
};

std::vector<Instruction> deserialize(uint8_t *data, int len);

std::pair<Instruction, int> decodeInstruction(uint8_t *src, int idx);

} // namespace Instructions
