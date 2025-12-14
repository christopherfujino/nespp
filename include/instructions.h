#pragma once

#include <array>
#include <cstdint>
#include <string>
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
  OpCodeType type = OpCodeType::unimplemented;
  AddressingMode addressing = AddressingMode::implied;

  std::string toString();
  bool operator==(OpCode other);
};

consteval std::pair<std::array<const char *, 256>, std::array<OpCode, 256>>
_buildOpCodeLookup() {
  std::array<OpCode, 256> table2 = {};
  std::array<const char *, 256> table1 = {};

  using enum OpCodeType;
  using enum AddressingMode;

  for (int i = 0; i < 256; ++i) {
    switch (i) {
    case 0x0A:
      table1[i] = "ASL";
      table2[i] = {.type = ASL, .addressing = accumulator};
      break;
    case 0x10:
      table1[i] = "BPL";
      table2[i] = {.type = BPL, .addressing = relative};
      break;
    case 0x20:
      table1[i] = "JSR";
      table2[i] = {.type = JSR, .addressing = absolute};
      break;
    case 0x2D:
      table1[i] = "AND";
      table2[i] = {.type = AND, .addressing = absolute};
      break;
    case 0x48:
      table1[i] = "PHA";
      table2[i] = {.type = PHA, .addressing = implied};
      break;
    case 0x4A:
      table1[i] = "LSR";
      table2[i] = {.type = LSR, .addressing = accumulator};
      break;
    case 0x4C:
      table1[i] = "JMP";
      table2[i] = {.type = JMP, .addressing = absolute};
      break;
    case 0x60:
      table1[i] = "RTS";
      table2[i] = {.type = RTS, .addressing = implied};
      break;
    case 0x6C:
      table1[i] = "JMP";
      table2[i] = {.type = JMP, .addressing = indirect};
      break;
    case 0x78:
      table1[i] = "SEI";
      table2[i] = {.type = SEI, .addressing = implied};
      break;
    case 0x85:
      table1[i] = "STA";
      table2[i] = {.type = STA, .addressing = zeropage};
      break;
    case 0x88:
      table1[i] = "DEY";
      table2[i] = {.type = DEY, .addressing = implied};
      break;
    case 0x8C:
      table1[i] = "STY";
      table2[i] = {.type = STY, .addressing = absolute};
      break;
    case 0x8D:
      table1[i] = "STA";
      table2[i] = {.type = STA, .addressing = absolute};
      break;
    case 0x8E:
      table1[i] = "STX";
      table2[i] = {.type = STX, .addressing = absolute};
      break;
    case 0x90:
      table1[i] = "BCC";
      table2[i] = {.type = BCC, .addressing = relative};
      break;
    case 0x9A:
      table1[i] = "TXS";
      table2[i] = {.type = TXS, .addressing = implied};
      break;
    case 0xA0:
      table1[i] = "LDY";
      table2[i] = {.type = LDY, .addressing = immediate};
      break;
    case 0xA2:
      table1[i] = "LDX";
      table2[i] = {.type = LDX, .addressing = immediate};
      break;
    case 0xA5:
      table1[i] = "LDA";
      table2[i] = {.type = LDA, .addressing = zeropage};
      break;
    case 0xA9:
      table1[i] = "LDA";
      table2[i] = {.type = LDA, .addressing = immediate};
      break;
    case 0xAA:
      table1[i] = "TAX";
      table2[i] = {.type = TAX, .addressing = implied};
      break;
    case 0xAD:
      table1[i] = "LDA";
      table2[i] = {.type = LDA, .addressing = absolute};
      break;
    case 0xB0:
      table1[i] = "BCS";
      table2[i] = {.type = BCS, .addressing = relative};
      break;
    case 0xBD:
      table1[i] = "LDA";
      table2[i] = {.type = LDA, .addressing = absolute};
      break;
    case 0xCA:
      table1[i] = "DEX";
      table2[i] = {.type = DEX, .addressing = implied};
      break;
    case 0xC6:
      table1[i] = "DEC";
      table2[i] = {.type = DEC, .addressing = zeropage};
      break;
    case 0xC8:
      table1[i] = "INY";
      table2[i] = {.type = INY, .addressing = zeropage};
      break;
    case 0xC9:
      table1[i] = "CMP";
      table2[i] = {.type = CMP, .addressing = immediate};
      break;
    case 0xD0:
      table1[i] = "BNE";
      table2[i] = {.type = BNE, .addressing = relative};
      break;
    case 0xD8:
      table1[i] = "CLD";
      table2[i] = {.type = CLD, .addressing = implied};
      break;
    case 0xE0:
      table1[i] = "CPX";
      table2[i] = {.type = CPX, .addressing = immediate};
      break;
    case 0xE6:
      table1[i] = "INC";
      table2[i] = {.type = INC, .addressing = zeropage};
      break;
    case 0xE8:
      table1[i] = "INX";
      table2[i] = {.type = INX, .addressing = implied};
      break;
    case 0xF0:
      table1[i] = "BEQ";
      table2[i] = {.type = BEQ, .addressing = relative};
      break;
    default:
      table1[i] = "unimplemented";
      table2[i] = {.type = unimplemented, .addressing = implied};
      break;
    }
  }

  return std::pair{table1, table2};
}

constexpr auto opCodeLookupPair = _buildOpCodeLookup();
constexpr auto opCodeNameLookup = opCodeLookupPair.first;
constexpr auto opCodeLookup = opCodeLookupPair.second;

union InstructionOperandUnion {
  Address::Absolute absolute;
  uint8_t immediate;
  void *implied;
  void *accumulator;
  uint8_t relative;
  uint8_t zeropage;
  Address::Absolute indirect;
};

class Instruction {
public:
  // Default no-op
  Instruction() : operand{.implied = nullptr} {}

  Instruction(OpCode opCode, InstructionOperandUnion operand)
      : opCode(opCode), operand(operand) {}

  OpCode opCode;
  InstructionOperandUnion operand;

  std::string toString();
};

// TODO: do we even need this anymore?
Instruction decodeInstruction(uint8_t **src, int idx);

} // namespace Instructions
