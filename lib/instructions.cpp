#include "../include/instructions.h" // for OpCode, Instruction, OpCode::AN...
#include <format>
#include <stdio.h> // for snprintf
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
    break;
  case accumulator:
    instruction.operand = {.immediate = **src};
    src += 2;
    break;
  case immediate:
    instruction.operand = {.immediate = **src};
    src += 2;
    break;
  case implied:
    instruction.operand = {.implied = nullptr};
    src += 1;
    break;
  case indirect:
    instruction.operand = {.absolute = {
                               *(*src + 2),
                               *(*src + 1),
                           }};
    src += 3;
    break;
  case relative:
    instruction.operand = {.relative = **src};
    src += 2;
    break;
  case zeropage:
    instruction.operand = {.zeropage = **src};
    src += 2;
    break;
  }
  return instruction;
}

bool OpCode::operator==(OpCode other) {
  return (type == other.type && addressing == other.addressing);
}

std::string OpCode::toString() {
  int opcode = -1;
  const char *operand = nullptr;

  for (int i = 0; i <= 0xFF; i++) {
    Instructions::OpCode current = opCodeLookup[i];
    if (*this == current) {
      opcode = i;
      break;
    }
  }

  if (opcode == -1) {
    throw std::runtime_error("BUG");
  }

  using enum AddressingMode;
  switch (addressing) {
  case absolute:
    operand = "absolute";
    break;
  case accumulator:
    operand = "accumulator";
    break;
  case immediate:
    operand = "immediate";
    break;
  case implied:
    operand = "implied";
    break;
  case indirect:
    operand = "indirect";
    break;
  case relative:
    operand = "relative";
    break;
  case zeropage:
    operand = "zeropage";
    break;
  }

  return std::format("0x{:2X} {}", opcode, operand);
}

// TODO remove this
Instruction decodeInstruction(uint8_t **src,
                              // TODO remove and handle error at higher level
                              int idx) {
  return _make(opCodeLookup[**src], src);
}

std::string Instruction::toString() {
  // TODO serialize operand
  return std::format("{}", opCode.toString());
}

} // namespace Instructions
