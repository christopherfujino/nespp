#pragma once

#include <array>
#include <stdint.h>
#include <string>
#include <utility>

#include "word.h"

enum OpCodeType {
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
  CPY, // Compare X with memory (Y - M)
  DEC, // Decrement value at operand address
  DEX, // Decrement X
  DEY, // Decrement Y
  INC, // Increment value at operand address
  INX, // Increment X
  INY, // Increment Y
  JMP, // Jump
  JSR, // Jump subroutine
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

// TODO namespace these
enum AddressingMode {
  absolute,
  accumulator,
  immediate,
  implied,
  indirect,
  relative,
  zeropage,
};

struct OpCode {
  OpCodeType type = unimplemented;
  AddressingMode addressing = implied;

  std::string toString();
  bool operator==(OpCode other);
};

typedef struct LookupTables {
  const char *names[256];
  OpCode opCodes[256];
} LookupTables;

// Can this be hidden?
extern LookupTables opCodeLookupPair;
extern const char **opCodeNameLookup;
extern OpCode *opCodeLookup;

union InstructionOperandUnion {
  Word absolute;
  uint8_t immediate;
  void *implied;
  void *accumulator;
  uint8_t relative;
  uint8_t zeropage;
  Word indirect;
};

struct Instruction {
  OpCode opCode;
  InstructionOperandUnion operand;
};

static inline Instruction instructionNew(OpCode opCode, InstructionOperandUnion operand) {
  return Instruction{
    .opCode = opCode,
    .operand = operand,
  };
}

std::string instructionToString(Instruction *);

// TODO: delete VM::decodeInstruction
Instruction decodeInstruction(uint8_t **src, int idx);
