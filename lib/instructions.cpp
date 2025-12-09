#include "../include/instructions.h" // for OpCode, Instruction, OpCode::AN...
#include "../include/address.h"      // for Absolute
#include <stdio.h>                   // for snprintf

namespace Instructions {

Instruction _make(OpCode opCode, uint8_t **src) {
  using enum AddressingMode;
  // TODO only check #ifdef NDEBUG
  if (opCode.type == OpCodeType::unimplemented) {
    char *msg = new char[256];
    snprintf(msg, 256, "Unimplemented instruction 0x%02X", **src);
    throw msg;
  }
  Instruction instruction = {
      .opCode = opCode,
  };
  switch (opCode.addressing) {
  case absolute:
    instruction.operand = {.absolute = Address::Absolute{
                               .low = *(*src + 1),
                               .high = *(*src + 2),
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
    instruction.operand = {.absolute = Address::Absolute{
                               .low = *(*src + 1),
                               .high = *(*src + 2),
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

Instruction
decodeInstruction(uint8_t **src,
                  int idx /* TODO remove and handle error at higher level */) {
  using enum OpCodeType;
  using enum AddressingMode;

  return _make(opCodeLookup[**src], src);
  // switch (**src) {
  // case 0x0A:
  //   // ASL_A
  //   return _make(ASL, accumulator, src);
  // case 0x10:
  //   return _make(BPL, relative, src);
  // case 0x20:
  //   return _make(JSR, absolute, src);
  // case 0x2D:
  //   return _make(AND, absolute, src);
  // case 0x48:
  //   return _make(PHA, implied, src);
  // case 0x4A:
  //   return _make(LSR, accumulator, src);
  // case 0x90:
  //   return _make(BCC, relative, src);
  // default:
  //   // This is never freed
  //   char *msg = new char[256];
  //   snprintf(msg, 256, "Unimplemented instruction 0x%02X at 0x%04X", **src,
  //            idx);
  //   throw msg;
  // }
}

} // namespace Instructions
