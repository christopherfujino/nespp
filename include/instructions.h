#pragma once

#include <cstdint>
#include <format>
#include <utility>
#include <vector>

#include "address.h"

namespace Instructions {

enum OpCode : uint8_t {
  AND_ABS = 0x2D, // A & M -> A
  JMP_ABS = 0x4C,
  JSR_ABS = 0x20,
  LDA_ABS = 0xAD,
  STA_ABS = 0x8D,
  STX_ABS = 0x8E,
  STY_ABS = 0x8C,
  LDA_ABS_X = 0xBD,
  JMP_INDIRECT = 0x6C,
  CMP_IMM = 0xC9, // Compare memory with accumulator (A - M)
  CPX_IMM = 0xE0, // Compare X with memory (X - M)
  LDA_IMM = 0xA9,
  LDX_IMM = 0xA2,
  LDY_IMM = 0xA0,
  BCC_REL = 0x90, // Branch on carry clear
  BCS_REL = 0xB0, // Branch on carry set
  BEQ_REL = 0xF0, // Branch on result equal to zero (Z = 1)
  BNE_REL = 0xD0, // Branch on not equal to zero
  BPL_REL = 0x10,
  INC_ZERO = 0xE6, // Increment memory by one
  DEC_ZERO = 0xC6, // Decrement memory by one
  LDA_ZERO = 0xA5,
  STA_ZERO = 0x85,
  ASL_A = 0x0A, // airthmetic shift left accumulator
  LSR_A = 0x4A, // logical shift right accumulator
  CLD = 0xD8, // Clear decimal
  DEX = 0xCA, // Decrement X by one
  DEY = 0x88, // Decrement Y by one
  INX = 0xE8, // Increment X by one
  INY = 0xC8, // Increment Y by one
  PHA = 0x48, // Push accumulator on stack
  RTS = 0x60, // return from subroutine
  SEI = 0x78, // set interrupt disable
  TAX = 0xAA, // Transfer accumulator to X
  TXS = 0x9A, // Transfer index X to stack register
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

std::vector<Instruction> deserialize(uint8_t *data, int len);

std::pair<Instruction, int> decodeInstruction(uint8_t *src, int idx);

    Instructions::Instruction current;
} // namespace Instructions
