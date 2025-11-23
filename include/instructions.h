#pragma once

#include <cstdint>
#include <utility>
#include <vector>

#include "address.h"

namespace Instructions {

enum class OpCode : uint8_t {
  ASL_A = 0x0A, // arithmetic shift left accumulator
  BPL_REL = 0x10, // Branch on result plus (N == 0)
  JSR_ABS = 0x20,
  AND_ABS = 0x2D, // A & M -> A
  PHA = 0x48, // Push accumulator on stack
  LSR_A = 0x4A, // logical shift right accumulator
  JMP_ABS = 0x4C,
  RTS = 0x60, // return from subroutine
  JMP_INDIRECT = 0x6C,
  SEI = 0x78, // set interrupt disable
  STA_ZERO = 0x85,
  DEY = 0x88, // Decrement Y by one
  STA_ABS = 0x8D,
  STX_ABS = 0x8E,
  STY_ABS = 0x8C,
  BCC_REL = 0x90, // Branch on carry clear
  TXS = 0x9A, // Transfer index X to stack register
  LDY_IMM = 0xA0,
  LDX_IMM = 0xA2,
  LDA_ZERO = 0xA5,
  LDA_IMM = 0xA9,
  TAX = 0xAA, // Transfer accumulator to X
  LDA_ABS = 0xAD,
  BCS_REL = 0xB0, // Branch on carry set
  LDA_ABS_X = 0xBD,
  DEX = 0xCA, // Decrement X by one
  DEC_ZERO = 0xC6, // Decrement memory by one
  INY = 0xC8, // Increment Y by one
  CMP_IMM = 0xC9, // Compare memory with accumulator (A - M)
  BNE_REL = 0xD0, // Branch on not equal to zero
  CLD = 0xD8, // Clear decimal
  CPX_IMM = 0xE0, // Compare X with memory (X - M)
  INC_ZERO = 0xE6, // Increment memory by one
  INX = 0xE8, // Increment X by one
  BEQ_REL = 0xF0, // Branch on result equal to zero (Z == 1)
};

struct Instruction {
  OpCode opCode;
  union {
    Address::Absolute absolute;
    uint8_t immediate;
    void *implied;
    void *accumulator;
    uint8_t relative;
    uint8_t zeropage;
    Address::Absolute indirect;
  } operand;
};

std::pair<Instruction, int> decodeInstruction(uint8_t *src, int idx);

} // namespace Instructions
