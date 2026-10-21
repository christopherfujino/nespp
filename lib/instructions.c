#include "instructions.h" // OpCode, Instruction, OpCode::AN...
#include "common.h"
// #include "word.h"

static inline LookupTables _buildOpCodeLookup() {
  LookupTables tables = {};
  auto names = tables.names;
  auto codes = tables.opCodes;

  for (int i = 0; i < 256; ++i) {
    names[i] = "unimplemented";
    codes[i] = (OpCode){.type = unimplemented, .addressing = implied};
  }

  names[0x0A] = "ASL";
  codes[0x0A] = (OpCode){.type = ASL, .addressing = accumulator};

  names[0x10] = "BPL";
  codes[0x10] = (OpCode){.type = BPL, .addressing = relative};

  names[0x20] = "JSR";
  codes[0x20] = (OpCode){.type = JSR, .addressing = absolute};

  names[0x2D] = "AND";
  codes[0x2D] = (OpCode){.type = AND, .addressing = absolute};

  names[0x48] = "PHA";
  codes[0x48] = (OpCode){.type = PHA, .addressing = implied};

  names[0x4A] = "LSR";
  codes[0x4A] = (OpCode){.type = LSR, .addressing = accumulator};

  names[0x4C] = "JMP";
  codes[0x4C] = (OpCode){.type = JMP, .addressing = absolute};

  names[0x60] = "RTS";
  codes[0x60] = (OpCode){.type = RTS, .addressing = implied};

  names[0x6C] = "JMP";
  codes[0x6C] = (OpCode){.type = JMP, .addressing = indirect};

  names[0x78] = "SEI";
  codes[0x78] = (OpCode){.type = SEI, .addressing = implied};

  names[0x85] = "STA";
  codes[0x85] = (OpCode){.type = STA, .addressing = zeropage};

  names[0x88] = "DEY";
  codes[0x88] = (OpCode){.type = DEY, .addressing = implied};

  names[0x8C] = "STY";
  codes[0x8C] = (OpCode){.type = STY, .addressing = absolute};

  names[0x8D] = "STA";
  codes[0x8D] = (OpCode){.type = STA, .addressing = absolute};

  names[0x8E] = "STX";
  codes[0x8E] = (OpCode){.type = STX, .addressing = absolute};

  names[0x90] = "BCC";
  codes[0x90] = (OpCode){.type = BCC, .addressing = relative};

  names[0x9A] = "TXS";
  codes[0x9A] = (OpCode){.type = TXS, .addressing = implied};

  names[0xA0] = "LDY";
  codes[0xA0] = (OpCode){.type = LDY, .addressing = immediate};

  names[0xA2] = "LDX";
  codes[0xA2] = (OpCode){.type = LDX, .addressing = immediate};

  names[0xA5] = "LDA";
  codes[0xA5] = (OpCode){.type = LDA, .addressing = zeropage};

  names[0xA9] = "LDA";
  codes[0xA9] = (OpCode){.type = LDA, .addressing = immediate};

  names[0xAA] = "TAX";
  codes[0xAA] = (OpCode){.type = TAX, .addressing = implied};

  names[0xAD] = "LDA";
  codes[0xAD] = (OpCode){.type = LDA, .addressing = absolute};

  names[0xB0] = "BCS";
  codes[0xB0] = (OpCode){.type = BCS, .addressing = relative};

  names[0xBD] = "LDA";
  codes[0xBD] = (OpCode){.type = LDA, .addressing = absolute};

  names[0xCA] = "DEX";
  codes[0xCA] = (OpCode){.type = DEX, .addressing = implied};

  names[0xC6] = "DEC";
  codes[0xC6] = (OpCode){.type = DEC, .addressing = zeropage};

  names[0xC8] = "INY";
  codes[0xC8] = (OpCode){.type = INY, .addressing = zeropage};

  names[0xC9] = "CMP";
  codes[0xC9] = (OpCode){.type = CMP, .addressing = immediate};

  names[0xD0] = "BNE";
  codes[0xD0] = (OpCode){.type = BNE, .addressing = relative};

  names[0xD8] = "CLD";
  codes[0xD8] = (OpCode){.type = CLD, .addressing = implied};

  names[0xE0] = "CPX";
  codes[0xE0] = (OpCode){.type = CPX, .addressing = immediate};

  names[0xE6] = "INC";
  codes[0xE6] = (OpCode){.type = INC, .addressing = zeropage};

  names[0xE8] = "INX";
  codes[0xE8] = (OpCode){.type = INX, .addressing = implied};

  names[0xF0] = "BEQ";
  codes[0xF0] = (OpCode){.type = BEQ, .addressing = relative};

  return tables;
}

// LookupTables opCodeLookupPair = _buildOpCodeLookup();
// const char **opCodeNameLookup = opCodeLookupPair.names;
// OpCode *opCodeLookup = opCodeLookupPair.opCodes;

void initModule() {
  opCodeLookupPair = _buildOpCodeLookup();
  opCodeNameLookup = opCodeLookupPair.names;
  opCodeLookup = opCodeLookupPair.opCodes;
}

Instruction _make(OpCode opCode, uint8_t **src) {
  if (opCode.type == OpCodeTypeUnimplemented) {
    panic("Unimplemented instruction 0x%02X", **src);
  }
  Instruction instruction = {};
  instruction.opCode = opCode;

  switch (opCode.addressing) {
  case absolute:
    instruction.operand.absolute = (Word){
        .high = *(*src + 2),
        .low = *(*src + 1),
    };
    src += 3;
    break;
  case accumulator:
    instruction.operand.immediate = **src;
    src += 2;
    break;
  case immediate:
    instruction.operand.immediate = **src;
    src += 2;
    break;
  case implied:
    instruction.operand.implied = nullptr;
    src += 1;
    break;
  case indirect:
    instruction.operand.absolute = (Word){
        .high = *(*src + 2),
        .low = *(*src + 1),
    };
    src += 3;
    break;
  case relative:
    instruction.operand.relative = **src;
    src += 2;
    break;
  case zeropage:
    instruction.operand.zeropage = **src;
    src += 2;
    break;
  }
  return instruction;
}

bool opCodeIsEqual(OpCode first, OpCode second) {
  return (first.type == second.type && first.addressing == second.addressing);
}

Result opCodeToString(OpCode *code, char **that) {
  int opcode = -1;

  for (int i = 0; i <= 0xFF; i++) {
    OpCode current = opCodeLookup[i];
    if (opCodeIsEqual(*code, current)) {
      opcode = i;
      break;
    }
  }

  if (opcode == -1) {
    return resultErr();
  }

  chrisprintf(that, "%s (%02X)", opCodeNameLookup[opcode], opcode);
  return resultOk();
}

std::string instructionToString(Instruction *instruction) {
  char *opCodeString = nullptr;
  opCodeToString(&instruction->opCode, &opCodeString);
  switch (instruction->opCode.addressing) {
  case absolute:
    // TODO: is this the right order?
    return std::format("{} {:02X} {:02X}", opCodeString,
                       instruction->operand.absolute.low,
                       instruction->operand.absolute.high);
  case accumulator:
    return std::format("{}  A", opCodeString);
  case immediate:
    return std::format("{} #{:02X}", opCodeString,
                       instruction->operand.immediate);
  case implied:
    return opCodeString;
  case indirect:
    return std::format("{}  ({:02X} {:02X})", opCodeString,
                       instruction->operand.indirect.low,
                       instruction->operand.indirect.high);
  case relative:
    return std::format("{}  {:02X}", opCodeString,
                       instruction->operand.relative);
  case zeropage:
    return std::format("{}  {:02X}", opCodeString,
                       instruction->operand.zeropage);
  }
  return std::format("{}", opCodeString);
}
