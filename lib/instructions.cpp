#include "../include/instructions.h" // for OpCode, Instruction, OpCode::AN...
#include <stdio.h>                   // for snprintf
#include <string>

namespace Instructions {

Instruction _make(OpCode opCode, uint8_t **src) {
  using enum AddressingMode;
  // TODO only check #ifdef NDEBUG
  if (opCode.type == OpCodeType::unimplemented) {
    char *msg = new char[256];
    snprintf(msg, 256, "Unimplemented instruction 0x%02X", **src);
    throw msg;
  }
  Instruction instruction = {};
  instruction.opCode = opCode;

  switch (opCode.addressing) {
  case absolute:
    instruction.operand = {.absolute = {
                               // high
                               *(*src + 2),
                               // low
                               *(*src + 1),
                           }};
    src += 3;
    return instruction;
  case accumulator:
    instruction.operand = {.immediate = **src};
    src += 2;
    return instruction;
  case immediate:
    instruction.operand = {.immediate = **src};
    src += 2;
    return instruction;
  case implied:
    instruction.operand = {.implied = nullptr};
    src += 1;
    return instruction;
  case indirect:
    instruction.operand = {.absolute = {
                               *(*src + 2),
                               *(*src + 1),
                           }};
    src += 3;
    return instruction;
  case relative:
    instruction.operand = {.relative = **src};
    src += 2;
    return instruction;
  case zeropage:
    instruction.operand = {.zeropage = **src};
    src += 2;
    return instruction;
  }
}

// TODO remove this
Instruction decodeInstruction(uint8_t **src,
                              // TODO remove and handle error at higher level
                              int idx) {
  using enum OpCodeType;
  using enum AddressingMode;

  return _make(opCodeLookup[**src], src);
}

std::string Instruction::toString() {
  return "TODO: implement Instruction::toString()";
}

} // namespace Instructions
