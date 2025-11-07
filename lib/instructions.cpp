#include <cstring>

#include "../include/debug.h"
#include "../include/instructions.h"

namespace Instructions {

std::vector<Instruction> deserialize(uint8_t *src, int len) {
  int index = 0;
  uint8_t *dataPtr = src;
  std::vector<Instruction> instructions;

  while (index < len) {
    std::pair<Instruction, int> pair =
        decodeInstruction(dataPtr + index, index);
    instructions.push_back(pair.first);
    printf("%04X: ", index);
    Debug::instruction(pair.first);
    index += pair.second;
  }
  // TODO: this will copy
  return instructions;
}

std::pair<Instruction, int> _makeAbsolutePair(uint8_t opCode, uint8_t lowByte,
                                              uint8_t highByte) {
  return std::make_pair(
      Instruction{.opCode = (OpCode)opCode,
                  .operand = {.absolute = AbsoluteAddress{.low = lowByte,
                                                          .high = highByte}}},
      3);
}

std::pair<Instruction, int> _makeImmediatePair(uint8_t opCode, uint8_t byte) {
  return std::make_pair(
      Instruction{.opCode = (OpCode)opCode, .operand = {.immediate = byte}}, 2);
}

std::pair<Instruction, int> _makeZeropagePair(uint8_t opCode, uint8_t byte) {
  return std::make_pair(
      Instruction{.opCode = (OpCode)opCode, .operand = {.zeropage = byte}}, 2);
}

std::pair<Instruction, int> _makeRelativePair(uint8_t opCode, uint8_t byte) {
  return std::make_pair(
      Instruction{.opCode = (OpCode)opCode, .operand = {.relative = byte}}, 2);
}

std::pair<Instruction, int> _makeImpliedPair(uint8_t opCode) {
  return std::make_pair(
      Instruction{.opCode = (OpCode)opCode, .operand = {.implied = nullptr}},
      1);
}

std::pair<Instruction, int> decodeInstruction(uint8_t *src, int idx) {
  switch (*src) {
  case LDA_ABS:
  case JMP_ABS:
  case STA_ABS:
  case STX_ABS:
  case LDA_ABS_X:
    return _makeAbsolutePair(*src, *(src + 1), *(src + 2));
  case BPL_REL:
  case BNE_REL:
    return _makeRelativePair(*src, *(src + 1));
  case CLD:
  case DEX:
  case DEY:
  case INX:
  case INY:
  case RTS:
  case SEI:
  case TXS:
    return _makeImpliedPair(*src);
  case CPX_IMM:
  case LDA_IMM:
  case LDX_IMM:
  case LDY_IMM:
    return _makeImmediatePair(*src, *(src + 1));
  case STA_ZERO:
    return _makeZeropagePair(*src, *(src + 1));
  default:
    // This is never freed
    char *msg = new char[256];
    snprintf(msg, 256, "Unimplemented instruction 0x%02X at 0x%04X", *src, idx);
    throw msg;
  }
}

} // namespace Instructions
