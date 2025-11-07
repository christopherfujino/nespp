#include "../include/debug.h"

namespace Debug {

void _printAbsoluteInstruction(const char *name,
                               Instructions::Instruction instruction) {
  printf("%s (%02X)  %02X %02X\n", name, instruction.opCode,
         instruction.operand.absolute.low, instruction.operand.absolute.high);
}

void _printImmediateInstruction(const char *name,
                                Instructions::Instruction instruction) {
  printf("%s (%02X) #%02X\n", name, instruction.opCode,
         instruction.operand.immediate);
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
  case Instructions::OpCode::LDA_IMM:
    _printImmediateInstruction("LDA", instruction);
    break;
  case Instructions::OpCode::LDX_IMM:
    _printImmediateInstruction("LDX", instruction);
    break;
  case Instructions::OpCode::LDA_ABS:
    _printAbsoluteInstruction("LDA", instruction);
    break;
  case Instructions::OpCode::JMP_ABS:
    _printAbsoluteInstruction("JMP", instruction);
    break;
  case Instructions::OpCode::STA_ABS:
    _printAbsoluteInstruction("STA", instruction);
    break;
  case Instructions::OpCode::BPL_REL:
    _printRelativeInstruction("BPL", instruction);
    break;
  case Instructions::OpCode::CLD:
    _printImpliedInstruction("CLD", instruction);
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
