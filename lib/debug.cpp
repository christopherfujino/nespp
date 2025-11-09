#include "../include/debug.h"
#include <cstring> // for strcmp()

namespace Debug {

void _printAbsoluteInstruction(const char *name,
                               Instructions::Instruction instruction) {
  printf("%s (%02X)  %02X %02X\n", name, (uint8_t)instruction.opCode,
         instruction.operand.absolute.low, instruction.operand.absolute.high);
}

void _printAbsoluteXInstruction(const char *name,
                                Instructions::Instruction instruction) {
  printf("%s (%02X)  %02X %02X,X\n", name, (uint8_t)instruction.opCode,
         instruction.operand.absolute.low, instruction.operand.absolute.high);
}

void _printIndirectInstruction(const char *name,
                               Instructions::Instruction instruction) {
  printf("%s (%02X) (%02X %02X)\n", name, (uint8_t)instruction.opCode,
         instruction.operand.absolute.low, instruction.operand.absolute.high);
}

void _printImmediateInstruction(const char *name,
                                Instructions::Instruction instruction) {
  printf("%s (%02X) #%02X\n", name, (uint8_t)instruction.opCode,
         instruction.operand.immediate);
}

void _printAccumulatorInstruction(const char *name,
                                  Instructions::Instruction instruction) {
  printf("%s (%02X)  A\n", name, (uint8_t)instruction.opCode);
}

void _printZeropageInstruction(const char *name,
                               Instructions::Instruction instruction) {
  printf("%s (%02X)  %02X\n", name, (uint8_t)instruction.opCode,
         instruction.operand.zeropage);
}

void _printRelativeInstruction(const char *name,
                               Instructions::Instruction instruction) {
  printf("%s (%02X)  %02X\n", name, (uint8_t)instruction.opCode,
         instruction.operand.relative);
}

void _printImpliedInstruction(const char *name,
                              Instructions::Instruction instruction) {
  printf("%s (%02X)\n", name, (uint8_t)instruction.opCode);
}

void instruction(Instructions::Instruction instruction) {
  switch (instruction.opCode) {
  case Instructions::OpCode::CMP_IMM:
    _printImmediateInstruction("CMP", instruction);
    break;
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
  case Instructions::OpCode::AND_ABS:
    _printAbsoluteInstruction("AND", instruction);
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
  case Instructions::OpCode::BCC_REL:
    _printRelativeInstruction("BCC", instruction);
    break;
  case Instructions::OpCode::BCS_REL:
    _printRelativeInstruction("BCS", instruction);
    break;
  case Instructions::OpCode::BEQ_REL:
    _printRelativeInstruction("BEQ", instruction);
    break;
  case Instructions::OpCode::BPL_REL:
    _printRelativeInstruction("BPL", instruction);
    break;
  case Instructions::OpCode::BNE_REL:
    _printRelativeInstruction("BNE", instruction);
    break;
  case Instructions::OpCode::JMP_INDIRECT:
    _printIndirectInstruction("JMP", instruction);
    break;
  case Instructions::OpCode::INC_ZERO:
    _printZeropageInstruction("INC", instruction);
    break;
  case Instructions::OpCode::DEC_ZERO:
    _printZeropageInstruction("DEC", instruction);
    break;
  case Instructions::OpCode::LDA_ZERO:
    _printZeropageInstruction("LDA", instruction);
    break;
  case Instructions::OpCode::STA_ZERO:
    _printZeropageInstruction("STA", instruction);
    break;
  case Instructions::OpCode::ASL_A:
    _printAccumulatorInstruction("ASL", instruction);
    break;
  case Instructions::OpCode::LSR_A:
    _printAccumulatorInstruction("LSR", instruction);
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
  case Instructions::OpCode::PHA:
    _printImpliedInstruction("PHA", instruction);
    break;
  case Instructions::OpCode::RTS:
    _printImpliedInstruction("RTS", instruction);
    break;
  case Instructions::OpCode::SEI:
    _printImpliedInstruction("SEI", instruction);
    break;
  case Instructions::OpCode::TAX:
    _printImpliedInstruction("TAX", instruction);
    break;
  case Instructions::OpCode::TXS:
    _printImpliedInstruction("TXS", instruction);
    break;
  }
}

void Debugger::start() {
  {
    uint8_t addressLow = vm->peek16(0xFFFC);
    uint8_t addressHigh = vm->peek16(0xFFFD);

    vm->PC = addressLow | (addressHigh << 8);
  }

  printf("%04X: Start\n", vm->PC);
  Instructions::Instruction ins;
  while (1) {
    printf("%04X: ", vm->PC);
    ins = vm->decodeInstruction();
    instruction(ins);
    vm->execute(ins);

    // TODO: this is sus, since getline expects this to be malloc allocated
    char inputLine[1024] = {0};
    char *inputLinePtr = inputLine;
    size_t inputSize = 1024;

    printf("? ");
    int nread = getline(&inputLinePtr, &inputSize, stdin);
    if (nread == EOF) {
      printf("\n\nExiting debugger.\n");
      exit(0);
    }
    if (nread == 1) {
      // step into
      continue;
    } else if (strncmp(inputLine, "setppu2", 5) == 0) {

    } else {
      printf("strncmp() = %d\n", strncmp(inputLine, "A = ", 4));
      printf("nread = %d\t\"%02X %02X\"\n", nread, inputLine[0], inputLine[1]);
      throw "Unrecognized debugger input";
    }
    printf("[%d | %ld] You typed: \"%s\"\n", nread, inputSize, inputLine);
  }
}

} // namespace Debug
