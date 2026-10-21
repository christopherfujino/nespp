#pragma once

#include <stdint.h>

#include "common.h"
#include "word.h"

typedef enum {
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
  OpCodeTypeUnimplemented,
} OpCodeType;

// TODO namespace these
typedef enum {
  absolute,
  accumulator,
  immediate,
  implied,
  indirect,
  relative,
  zeropage,
} AddressingMode;

typedef struct {
  OpCodeType type;
  AddressingMode addressing;
} OpCode;

bool opCodeIsEqual(OpCode first, OpCode second);
Result opCodeToString(OpCode *code, char **that);

typedef struct LookupTables {
  const char *names[256];
  OpCode opCodes[256];
} LookupTables;

// Can this be hidden?
extern LookupTables opCodeLookupPair;
extern const char **opCodeNameLookup;
extern OpCode *opCodeLookup;

typedef union {
  Word absolute;
  uint8_t immediate;
  void *implied;
  void *accumulator;
  uint8_t relative;
  uint8_t zeropage;
  Word indirect;
} InstructionOperandUnion;

typedef struct {
  OpCode opCode;
  InstructionOperandUnion operand;
} Instruction;

const char *instructionToString(Instruction *);

static inline Instruction instructionNew(OpCode opCode,
                                         InstructionOperandUnion operand) {
  return Instruction{
      .opCode = opCode,
      .operand = operand,
  };
}

const char *instructionToString(Instruction *);

// TODO: delete VM::decodeInstruction
Instruction decodeInstruction(uint8_t **src, int idx);
