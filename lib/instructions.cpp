#include "../include/instructions.h" // for OpCode, Instruction, OpCode::AN...
#include "../include/word.h"
#include <format>
#include <stdexcept>
#include <stdio.h> // for snprintf
#include <string>

namespace NESPP {

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
  std::string operandString;

  for (int i = 0; i <= 0xFF; i++) {
    OpCode current = opCodeLookup[i];
    if (*this == current) {
      opcode = i;
      break;
    }
  }

  if (opcode == -1) {
    throw std::runtime_error("BUG");
  }
  return std::format("{} ({:02X})", opCodeNameLookup[opcode], opcode);
}

std::string Instruction::toString() {
  using enum AddressingMode;
  switch (opCode.addressing) {
  case absolute:
    // TODO: is this the right order?
    return std::format("{} {:02X} {:02X}", opCode.toString(),
                       operand.absolute.low, operand.absolute.high);
  case accumulator:
    return std::format("{}  A", opCode.toString());
  case immediate:
    return std::format("{} #{:02X}", opCode.toString(), operand.immediate);
  case implied:
    return opCode.toString();
  case indirect:
    return std::format("{}  ({:02X} {:02X})", opCode.toString(),
                       operand.indirect.low, operand.indirect.high);
  case relative:
    return std::format("{}  {:02X}", opCode.toString(), operand.relative);
  case zeropage:
    return std::format("{}  {:02X}", opCode.toString(), operand.zeropage);
  }
  return std::format("{}", opCode.toString());
}

} // namespace NESPP
