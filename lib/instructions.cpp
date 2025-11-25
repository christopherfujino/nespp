#include "../include/instructions.h" // for OpCode, Instruction, OpCode::AN...
#include "../include/address.h"      // for Absolute
#include <stdio.h>                   // for snprintf
#include <utility>                   // for make_pair, pair

namespace Instructions {

std::pair<Instruction, int> _makeAbsolutePair(uint8_t opCode, uint8_t lowByte,
                                              uint8_t highByte) {
  return std::make_pair(
      Instruction{.opCode = (OpCode)opCode,
                  .operand = {.absolute = Address::Absolute{.low = lowByte,
                                                            .high = highByte}}},
      3);
}

std::pair<Instruction, int> _makeIndirectPair(uint8_t opCode, uint8_t lowByte,
                                              uint8_t highByte) {
  return std::make_pair(
      Instruction{.opCode = (OpCode)opCode,
                  .operand = {.indirect = Address::Absolute{.low = lowByte,
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

std::pair<Instruction, int> _makeAccumulatorPair(uint8_t opCode) {
  return std::make_pair(Instruction{.opCode = (OpCode)opCode,
                                    .operand = {.accumulator = nullptr}},
                        1);
}

std::pair<Instruction, int> _makeImpliedPair(uint8_t opCode) {
  return std::make_pair(
      Instruction{.opCode = (OpCode)opCode, .operand = {.implied = nullptr}},
      1);
}

std::pair<Instruction, int> decodeInstruction(uint8_t *src, int idx) {
  using enum OpCode;
  switch ((OpCode)*src) {
  case AND_ABS:
  case LDA_ABS:
  case JMP_ABS:
  case JSR_ABS:
  case STA_ABS:
  case STX_ABS:
  case STY_ABS:
  case LDA_ABS_X:
    return _makeAbsolutePair(*src, *(src + 1), *(src + 2));
  case BCC_REL:
  case BCS_REL:
  case BEQ_REL:
  case BNE_REL:
  case BPL_REL:
    return _makeRelativePair(*src, *(src + 1));
  case ASL_A:
  case LSR_A:
    return _makeAccumulatorPair(*src);
  case CLD:
  case DEX:
  case DEY:
  case INX:
  case INY:
  case PHA:
  case RTS:
  case SEI:
  case TAX:
  case TXS:
    return _makeImpliedPair(*src);
  case JMP_INDIRECT:
    return _makeIndirectPair(*src, *(src + 1), *(src + 2));
  case CMP_IMM:
  case CPX_IMM:
  case LDA_IMM:
  case LDX_IMM:
  case LDY_IMM:
    return _makeImmediatePair(*src, *(src + 1));
  case DEC_ZERO:
  case INC_ZERO:
  case LDA_ZERO:
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
