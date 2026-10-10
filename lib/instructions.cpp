#include "../include/instructions.h" // for OpCode, Instruction, OpCode::AN...
#include "../include/word.h"
#include <format>
#include <stdexcept>
#include <string>

static inline LookupTables _buildOpCodeLookup() {
  LookupTables tables;
  auto names = tables.names;
  auto codes = tables.opCodes;

  for (int i = 0; i < 256; ++i) {
    names[i] = "unimplemented";
    codes[i] = {.type = unimplemented, .addressing = implied};
  }

  names[0x0A] = "ASL";
  codes[0x0A] = {.type = ASL, .addressing = accumulator};

  names[0x10] = "BPL";
  codes[0x10] = {.type = BPL, .addressing = relative};

  names[0x20] = "JSR";
  codes[0x20] = {.type = JSR, .addressing = absolute};

  names[0x2D] = "AND";
  codes[0x2D] = {.type = AND, .addressing = absolute};

  names[0x48] = "PHA";
  codes[0x48] = {.type = PHA, .addressing = implied};

  names[0x4A] = "LSR";
  codes[0x4A] = {.type = LSR, .addressing = accumulator};

  names[0x4C] = "JMP";
  codes[0x4C] = {.type = JMP, .addressing = absolute};

  names[0x60] = "RTS";
  codes[0x60] = {.type = RTS, .addressing = implied};

  names[0x6C] = "JMP";
  codes[0x6C] = {.type = JMP, .addressing = indirect};

  names[0x78] = "SEI";
  codes[0x78] = {.type = SEI, .addressing = implied};

  names[0x85] = "STA";
  codes[0x85] = {.type = STA, .addressing = zeropage};

  names[0x88] = "DEY";
  codes[0x88] = {.type = DEY, .addressing = implied};

  names[0x8C] = "STY";
  codes[0x8C] = {.type = STY, .addressing = absolute};

  names[0x8D] = "STA";
  codes[0x8D] = {.type = STA, .addressing = absolute};

  names[0x8E] = "STX";
  codes[0x8E] = {.type = STX, .addressing = absolute};

  names[0x90] = "BCC";
  codes[0x90] = {.type = BCC, .addressing = relative};

  names[0x9A] = "TXS";
  codes[0x9A] = {.type = TXS, .addressing = implied};

  names[0xA0] = "LDY";
  codes[0xA0] = {.type = LDY, .addressing = immediate};

  names[0xA2] = "LDX";
  codes[0xA2] = {.type = LDX, .addressing = immediate};

  names[0xA5] = "LDA";
  codes[0xA5] = {.type = LDA, .addressing = zeropage};

  names[0xA9] = "LDA";
  codes[0xA9] = {.type = LDA, .addressing = immediate};

  names[0xAA] = "TAX";
  codes[0xAA] = {.type = TAX, .addressing = implied};

  names[0xAD] = "LDA";
  codes[0xAD] = {.type = LDA, .addressing = absolute};

  names[0xB0] = "BCS";
  codes[0xB0] = {.type = BCS, .addressing = relative};

  names[0xBD] = "LDA";
  codes[0xBD] = {.type = LDA, .addressing = absolute};

  names[0xCA] = "DEX";
  codes[0xCA] = {.type = DEX, .addressing = implied};

  names[0xC6] = "DEC";
  codes[0xC6] = {.type = DEC, .addressing = zeropage};

  names[0xC8] = "INY";
  codes[0xC8] = {.type = INY, .addressing = zeropage};

  names[0xC9] = "CMP";
  codes[0xC9] = {.type = CMP, .addressing = immediate};

  names[0xD0] = "BNE";
  codes[0xD0] = {.type = BNE, .addressing = relative};

  names[0xD8] = "CLD";
  codes[0xD8] = {.type = CLD, .addressing = implied};

  names[0xE0] = "CPX";
  codes[0xE0] = {.type = CPX, .addressing = immediate};

  names[0xE6] = "INC";
  codes[0xE6] = {.type = INC, .addressing = zeropage};

  names[0xE8] = "INX";
  codes[0xE8] = {.type = INX, .addressing = implied};

  names[0xF0] = "BEQ";
  codes[0xF0] = {.type = BEQ, .addressing = relative};

  return tables;
}

LookupTables opCodeLookupPair = _buildOpCodeLookup();
const char **opCodeNameLookup = opCodeLookupPair.names;
OpCode *opCodeLookup = opCodeLookupPair.opCodes;

Instruction _make(OpCode opCode, uint8_t **src) {
#ifndef NDEBUG
  if (opCode.type == OpCodeType::unimplemented) {
    abort();
    // char *msg = new char[256];
    // snprintf(msg, 256, "Unimplemented instruction 0x%02X", **src);
    // throw msg;
  }
#endif
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
