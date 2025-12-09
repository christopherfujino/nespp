#pragma once

#include <array>
#include <cstdint>
#include <utility>

#include "address.h"

namespace Instructions {

enum class OpCodeType {
  AND, /// & accumulator
  ASL, // arithmetic shift left
  BCC, // Branch on carry clear
  BCS, // Branch on carry set (C == 1)
  BEQ, // Branch on result equal to zero (Z == 1)
  BNE, // Branch on not equal to zero
  BPL, // Branch on result plus (N == 0)
  CLD, // Clear decimal status register
  CMP, // Compare memory with accumulator (A - M)
  CPX, // Compare X with memory (X - M)
  DEC, // Decrement value at operand address
  DEX, // Decrement X
  DEY, // Decrement Y
  INC, // Increment value at operand address
  INX, // Increment X
  INY, // Increment Y
  JMP, // Jump
  JSR,
  LDA,
  LDX,
  LDY,
  LSR, // Logical shift right
  PHA, // Push accumulator onto stack
  RTS, // Return from subroutine
  SEI, // Set interrupt disabled
  STA,
  STX,
  STY,
  TAX, // Transfer accumulator to X
  TXS, // Transfer index X to stack register
  unimplemented,
};

enum class AddressingMode {
  absolute,
  accumulator,
  immediate,
  implied,
  indirect,
  relative,
  zeropage,
};

struct OpCode {
  OpCodeType type;
  AddressingMode addressing;
};

consteval std::array<OpCode, 256> _buildOpCodeLookup() {
  std::array<OpCode, 256> table = {};

  using enum OpCodeType;
  using enum AddressingMode;

  for (int i = 0; i < 256; ++i) {
    switch (i) {
    case 0x0A:
      table[i] = {.type = ASL, .addressing = accumulator};
      break;
    case 0x10:
      table[i] = {.type = BPL, .addressing = relative};
      break;
    case 0x20:
      table[i] = {.type = JSR, .addressing = absolute};
      break;
    case 0x2D:
      table[i] = {.type = AND, .addressing = absolute};
      break;
    case 0x48:
      table[i] = {.type = PHA, .addressing = implied};
      break;
    case 0x4A:
      table[i] = {.type = LSR, .addressing = accumulator};
      break;
    case 0x4C:
      table[i] = {.type = JMP, .addressing = absolute};
      break;
    case 0x60:
      table[i] = {.type = RTS, .addressing = implied};
      break;
    case 0x6C:
      table[i] = {.type = JMP, .addressing = indirect};
      break;
    case 0x78:
      table[i] = {.type = SEI, .addressing = implied};
      break;
    case 0x85:
      table[i] = {.type = STA, .addressing = zeropage};
      break;
    case 0x88:
      table[i] = {.type = DEY, .addressing = implied};
      break;
    case 0x8C:
      table[i] = {.type = STY, .addressing = absolute};
      break;
    case 0x8D:
      table[i] = {.type = STA, .addressing = absolute};
      break;
    case 0x8E:
      table[i] = {.type = STX, .addressing = absolute};
      break;
    case 0x90:
      table[i] = {.type = BCC, .addressing = relative};
      break;
    case 0x9A:
      table[i] = {.type = TXS, .addressing = implied};
      break;
    case 0xA0:
      table[i] = {.type = LDY, .addressing = immediate};
      break;
    case 0xA2:
      table[i] = {.type = LDX, .addressing = immediate};
      break;
    case 0xA5:
      table[i] = {.type = LDA, .addressing = zeropage};
      break;
    case 0xA9:
      table[i] = {.type = LDA, .addressing = immediate};
      break;
    case 0xAA:
      table[i] = {.type = TAX, .addressing = implied};
      break;
    case 0xAD:
      table[i] = {.type = LDA, .addressing = absolute};
      break;
    case 0xB0:
      table[i] = {.type = BCS, .addressing = relative};
      break;
    case 0xBD:
      table[i] = {.type = LDA, .addressing = absolute};
      break;
    case 0xCA:
      table[i] = {.type = DEX, .addressing = implied};
      break;
    case 0xC6:
      table[i] = {.type = DEC, .addressing = zeropage};
      break;
    case 0xC8:
      table[i] = {.type = INY, .addressing = zeropage};
      break;
    case 0xC9:
      table[i] = {.type = CMP, .addressing = immediate};
      break;
    case 0xD0:
      table[i] = {.type = BNE, .addressing = relative};
      break;
    case 0xD8:
      table[i] = {.type = CLD, .addressing = implied};
      break;
    case 0xE0:
      table[i] = {.type = CPX, .addressing = immediate};
      break;
    case 0xE6:
      table[i] = {.type = INC, .addressing = zeropage};
      break;
    case 0xE8:
      table[i] = {.type = INX, .addressing = implied};
      break;
    case 0xF0:
      table[i] = {.type = BEQ, .addressing = relative};
      break;
    default:
      table[i] = {.type = unimplemented, .addressing = implied};
      break;
    }
  }

  return table;
}

constexpr std::array<OpCode, 256> opCodeLookup = _buildOpCodeLookup();

//enum class _OpCode : uint8_t {
  // ASL_A = 0x0A, // arithmetic shift left accumulator
  // BPL_REL = 0x10, // Branch on result plus (N == 0)
  // JSR_ABS = 0x20,
  // AND_ABS = 0x2D, // A & M -> A
  // PHA = 0x48, // Push accumulator on stack
  // LSR_A = 0x4A, // logical shift right accumulator
  // JMP_ABS = 0x4C,
  // RTS = 0x60, // return from subroutine
  // JMP_INDIRECT = 0x6C,
  // SEI = 0x78, // set interrupt disable
  // STA_ZERO = 0x85,
  // DEY = 0x88, // Decrement Y by one
  // STA_ABS = 0x8D,
  // STX_ABS = 0x8E,
  // STY_ABS = 0x8C,
  // BCC_REL = 0x90, // Branch on carry clear
  // TXS = 0x9A,     // Transfer index X to stack register
  // LDY_IMM = 0xA0,
  // LDX_IMM = 0xA2,
  // LDA_ZERO = 0xA5,
  // LDA_IMM = 0xA9,
  // TAX = 0xAA, // Transfer accumulator to X
  // LDA_ABS = 0xAD,
  // BCS_REL = 0xB0, // Branch on carry set
  // LDA_ABS_X = 0xBD,
  // DEX = 0xCA,      // Decrement X by one
  // DEC_ZERO = 0xC6, // Decrement memory by one
  // INY = 0xC8,      // Increment Y by one
  // CMP_IMM = 0xC9,  // Compare memory with accumulator (A - M)
  // BNE_REL = 0xD0,  // Branch on not equal to zero
  // CLD = 0xD8,      // Clear decimal
  // CPX_IMM = 0xE0,  // Compare X with memory (X - M)
  // INC_ZERO = 0xE6, // Increment memory by one
  // INX = 0xE8,      // Increment X by one
  // BEQ_REL = 0xF0,  // Branch on result equal to zero (Z == 1)
//};

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

// TODO: pointer -> pointer
Instruction decodeInstruction(uint8_t *src, int idx);

} // namespace Instructions
