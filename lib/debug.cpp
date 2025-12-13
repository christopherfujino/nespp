#include "../include/debug.h"
#include "../include/address.h"      // for Absolute
#include "../include/instructions.h" // for Instruction, OpCode
#include "../include/vm.h"           // for VM
#include <cstdlib>                   // for exit, size_t
#include <cstring>                   // for strncmp
#include <stdint.h>                  // for uint8_t
#include <stdio.h>                   // for printf, getline, EOF, stdin

namespace Debug {

// void _printAbsoluteInstruction(const char *name,
//                                Instructions::Instruction instruction) {
//   printf("%s (%02X)  %02X %02X\n", name, (uint8_t)instruction.opCode,
//          instruction.operand.absolute.low,
//          instruction.operand.absolute.high);
// }
//
// void _printAbsoluteXInstruction(const char *name,
//                                 Instructions::Instruction instruction) {
//   printf("%s (%02X)  %02X %02X,X\n", name, (uint8_t)instruction.opCode,
//          instruction.operand.absolute.low,
//          instruction.operand.absolute.high);
// }
//
// void _printIndirectInstruction(const char *name,
//                                Instructions::Instruction instruction) {
//   printf("%s (%02X) (%02X %02X)\n", name, (uint8_t)instruction.opCode,
//          instruction.operand.absolute.low,
//          instruction.operand.absolute.high);
// }
//
// void _printImmediateInstruction(const char *name,
//                                 Instructions::Instruction instruction) {
//   printf("%s (%02X) #%02X\n", name, (uint8_t)instruction.opCode,
//          instruction.operand.immediate);
// }
//
// void _printAccumulatorInstruction(const char *name,
//                                   Instructions::Instruction instruction) {
//   printf("%s (%02X)  A\n", name, (uint8_t)instruction.opCode);
// }
//
// void _printZeropageInstruction(const char *name,
//                                Instructions::Instruction instruction) {
//   printf("%s (%02X)  %02X\n", name, (uint8_t)instruction.opCode,
//          instruction.operand.zeropage);
// }
//
// void _printRelativeInstruction(const char *name,
//                                Instructions::Instruction instruction) {
//   printf("%s (%02X)  %02X\n", name, (uint8_t)instruction.opCode,
//          instruction.operand.relative);
// }
//
// void _printImpliedInstruction(const char *name,
//                               Instructions::Instruction instruction) {
//   printf("%s (%02X)\n", name, (uint8_t)instruction.opCode);
// }
//
// void instruction(Instructions::Instruction instruction) {
//   switch (instruction.opCode) {
//   case Instructions::OpCode::CMP_IMM:
//     _printImmediateInstruction("CMP", instruction);
//     break;
//   case Instructions::OpCode::CPX_IMM:
//     _printImmediateInstruction("CPX", instruction);
//     break;
//   case Instructions::OpCode::LDA_IMM:
//     _printImmediateInstruction("LDA", instruction);
//     break;
//   case Instructions::OpCode::LDX_IMM:
//     _printImmediateInstruction("LDX", instruction);
//     break;
//   case Instructions::OpCode::LDY_IMM:
//     _printImmediateInstruction("LDY", instruction);
//     break;
//   case Instructions::OpCode::AND_ABS:
//     _printAbsoluteInstruction("AND", instruction);
//     break;
//   case Instructions::OpCode::LDA_ABS:
//     _printAbsoluteInstruction("LDA", instruction);
//     break;
//   case Instructions::OpCode::LDA_ABS_X:
//     _printAbsoluteXInstruction("LDA", instruction);
//     break;
//   case Instructions::OpCode::JMP_ABS:
//     _printAbsoluteInstruction("JMP", instruction);
//     break;
//   case Instructions::OpCode::JSR_ABS:
//     _printAbsoluteInstruction("JSR", instruction);
//     break;
//   case Instructions::OpCode::STA_ABS:
//     _printAbsoluteInstruction("STA", instruction);
//     break;
//   case Instructions::OpCode::STX_ABS:
//     _printAbsoluteInstruction("STX", instruction);
//     break;
//   case Instructions::OpCode::STY_ABS:
//     _printAbsoluteInstruction("STY", instruction);
//     break;
//   case Instructions::OpCode::BCC_REL:
//     _printRelativeInstruction("BCC", instruction);
//     break;
//   case Instructions::OpCode::BCS_REL:
//     _printRelativeInstruction("BCS", instruction);
//     break;
//   case Instructions::OpCode::BEQ_REL:
//     _printRelativeInstruction("BEQ", instruction);
//     break;
//   case Instructions::OpCode::BPL_REL:
//     _printRelativeInstruction("BPL", instruction);
//     break;
//   case Instructions::OpCode::BNE_REL:
//     _printRelativeInstruction("BNE", instruction);
//     break;
//   case Instructions::OpCode::JMP_INDIRECT:
//     _printIndirectInstruction("JMP", instruction);
//     break;
//   case Instructions::OpCode::INC_ZERO:
//     _printZeropageInstruction("INC", instruction);
//     break;
//   case Instructions::OpCode::DEC_ZERO:
//     _printZeropageInstruction("DEC", instruction);
//     break;
//   case Instructions::OpCode::LDA_ZERO:
//     _printZeropageInstruction("LDA", instruction);
//     break;
//   case Instructions::OpCode::STA_ZERO:
//     _printZeropageInstruction("STA", instruction);
//     break;
//   case Instructions::OpCode::ASL_A:
//     _printAccumulatorInstruction("ASL", instruction);
//     break;
//   case Instructions::OpCode::LSR_A:
//     _printAccumulatorInstruction("LSR", instruction);
//     break;
//   case Instructions::OpCode::CLD:
//     _printImpliedInstruction("CLD", instruction);
//     break;
//   case Instructions::OpCode::DEX:
//     _printImpliedInstruction("DEX", instruction);
//     break;
//   case Instructions::OpCode::DEY:
//     _printImpliedInstruction("DEY", instruction);
//     break;
//   case Instructions::OpCode::INX:
//     _printImpliedInstruction("INX", instruction);
//     break;
//   case Instructions::OpCode::INY:
//     _printImpliedInstruction("INY", instruction);
//     break;
//   case Instructions::OpCode::PHA:
//     _printImpliedInstruction("PHA", instruction);
//     break;
//   case Instructions::OpCode::RTS:
//     _printImpliedInstruction("RTS", instruction);
//     break;
//   case Instructions::OpCode::SEI:
//     _printImpliedInstruction("SEI", instruction);
//     break;
//   case Instructions::OpCode::TAX:
//     _printImpliedInstruction("TAX", instruction);
//     break;
//   case Instructions::OpCode::TXS:
//     _printImpliedInstruction("TXS", instruction);
//     break;
//   }
// }

void _renderInstruction(Instructions::Instruction &ins) {
  printf("TODO %s\n", __FILE__);
}

void Debugger::start() {
  {
    uint8_t addressLow = vm->peek16(0xFFFC);
    uint8_t addressHigh = vm->peek16(0xFFFD);

    vm->PC = addressLow | (addressHigh << 8);
  }

  Instructions::Instruction ins;
  while (1) {
    printf("%04X: ", vm->PC.to16());
    ins = vm->decodeInstruction();
    _renderInstruction(ins);
    vm->execute(ins);
    printRegisters();

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
      // TODO: is this right?
      // we're branching on if the zero flag is set, so don't branch
      vm->ppuRegisters[2] = 1 << 7;
      printf("Setting PPU[2] = #%02X\n", vm->ppuRegisters[2]);
      continue;
    } else {
      printf("strncmp() = %d\n", strncmp(inputLine, "A = ", 4));
      printf("nread = %d\t\"%02X %02X\"\n", nread, inputLine[0], inputLine[1]);
      throw "Unrecognized debugger input";
    }
    printf("[%d | %ld] You typed: \"%s\"\n", nread, inputSize, inputLine);
  }
}

void Debugger::printRegisters() {
  printf("PC   A  X  Y  SP NV-BDIZC\n");
  printf("%04X %02X %02X %02X %02X %08b\n\n", vm->PC.to16(), vm->A, vm->X,
         vm->Y, vm->SP, vm->S);
}

} // namespace Debug
