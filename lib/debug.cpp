#include "../include/debug.h"

namespace Debug {

void _printAbsoluteInstruction(const char *name,
                               Instructions::Instruction instruction) {
  printf("%s (%02X)  %02X %02X\n", name, instruction.opCode,
         instruction.operand.absolute.low, instruction.operand.absolute.high);
}

void _printAbsoluteXInstruction(const char *name,
                                Instructions::Instruction instruction) {
  printf("%s (%02X)  %02X %02X,X\n", name, instruction.opCode,
         instruction.operand.absolute.low, instruction.operand.absolute.high);
}

void _printImmediateInstruction(const char *name,
                                Instructions::Instruction instruction) {
  printf("%s (%02X) #%02X\n", name, instruction.opCode,
         instruction.operand.immediate);
}

void _printZeropageInstruction(const char *name,
                               Instructions::Instruction instruction) {
  printf("%s (%02X) %02X\n", name, instruction.opCode,
         instruction.operand.zeropage);
}

void _printRelativeInstruction(const char *name,
                               Instructions::Instruction instruction) {
  printf("%s (%02X)  %02X\n", name, instruction.opCode,
         instruction.operand.relative);
}

void _printImpliedInstruction(const char *name,
                              Instructions::Instruction instruction) {
  printf("%s (%02X)\n", name, instruction.opCode);
}

void instruction(Instructions::Instruction instruction) {
  switch (instruction.opCode) {
  case Instructions::OpCode::CPX_IMM:
    _printImmediateInstruction("CPX", instruction);
    break;
  case Instructions::OpCode::LDA_IMM:
    _printImmediateInstruction("LDA", instruction);
    break;
  case Instructions::OpCode::LDX_IMM:
    _printImmediateInstruction("LDX", instruction);
    break;
  case Instructions::OpCode::LDY_IMM:
    _printImmediateInstruction("LDY", instruction);
    break;
  case Instructions::OpCode::LDA_ABS:
    _printAbsoluteInstruction("LDA", instruction);
    break;
  case Instructions::OpCode::LDA_ABS_X:
    _printAbsoluteXInstruction("LDA", instruction);
    break;
  case Instructions::OpCode::JMP_ABS:
    _printAbsoluteInstruction("JMP", instruction);
    break;
  case Instructions::OpCode::JSR_ABS:
    _printAbsoluteInstruction("JSR", instruction);
    break;
  case Instructions::OpCode::STA_ABS:
    _printAbsoluteInstruction("STA", instruction);
    break;
  case Instructions::OpCode::STX_ABS:
    _printAbsoluteInstruction("STX", instruction);
    break;
  case Instructions::OpCode::STY_ABS:
    _printAbsoluteInstruction("STY", instruction);
    break;
  case Instructions::OpCode::BPL_REL:
    _printRelativeInstruction("BPL", instruction);
    break;
  case Instructions::OpCode::BNE_REL:
    _printRelativeInstruction("BNE", instruction);
    break;
  case Instructions::OpCode::LDA_ZERO:
    _printZeropageInstruction("LDA", instruction);
    break;
  case Instructions::OpCode::STA_ZERO:
    _printZeropageInstruction("STA", instruction);
    break;
  case Instructions::OpCode::CLD:
    _printImpliedInstruction("CLD", instruction);
    break;
  case Instructions::OpCode::DEX:
    _printImpliedInstruction("DEX", instruction);
    break;
  case Instructions::OpCode::DEY:
    _printImpliedInstruction("DEY", instruction);
    break;
  case Instructions::OpCode::INX:
    _printImpliedInstruction("INX", instruction);
    break;
  case Instructions::OpCode::INY:
    _printImpliedInstruction("INY", instruction);
    break;
  case Instructions::OpCode::RTS:
    _printImpliedInstruction("RTS", instruction);
    break;
  case Instructions::OpCode::SEI:
    _printImpliedInstruction("SEI", instruction);
    break;
  case Instructions::OpCode::TXS:
    _printImpliedInstruction("TXS", instruction);
    break;
  }
}

} // namespace Debug
