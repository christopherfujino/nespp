#include <cstring>
#include <stdio.h>

#include "../include/debug.h"
#include "../include/vm.h"

namespace VM {

Mapper0::Mapper0(Rom *rom) {
  this->rom = rom;

  switch (rom->prgSize) {
  case 0x4000: // 16KiB
    memcpy(prg, rom->prgBlob, 0x4000);
    // Mirror
    memcpy(prg + 0x4000, rom->prgBlob, 0x4000);
    break;
  case 0x8000: // 32KiB
    memcpy(prg, rom->prgBlob, 0x8000);
    break;
  default:
    // This is never freed
    char *msg = new char[256];
    snprintf(msg, 256, "Unknown PRG size %04X\n", rom->prgSize);
    throw msg;
  }
}

Mapper0::~Mapper0() {
  // Note: don't delete this->rom, we don't own it.
}

uint8_t Mapper0::peek16(uint16_t address) {
  if (address < 0x6000) {
    throw "Unreachable";
  } else if (address < 0x8000) {
    // unbanked PRG-RAM
    throw "TODO: implement PRG-RAM";
  } else if (address <= 0xFFFF) {
    // either continuation of PRG or mirror
    uint16_t offset = address - 0x8000;
    return prg[offset];
  }

  throw "Unreachable";
}

VM::VM(Rom *rom) {
  printf("Constructing a VM...\n");
  switch (rom->mapper) {
  case 0:
    mapper = new Mapper0(rom);
  }
}

VM::~VM() { delete mapper; }

void VM::start() {
  {
    uint8_t addressLow = peek16(0xFFFC);
    printf("Peeking 0xFFFC = %02X\n", addressLow);

    uint8_t addressHigh = peek16(0xFFFD);
    printf("Peeking 0xFFFD = %02X\n", addressHigh);

    PC = addressLow | (addressHigh << 8);
  }

  Instructions::Instruction current;
  while (1) {
    current = decodeInstruction();

    Debug::instruction(current);
  }
}

uint8_t VM::peek(Address::Absolute address) {
  return peek16(address.low | (address.high << 8));
}

uint8_t VM::peek8(uint8_t offset) { return ram[offset]; }

uint8_t VM::peek16(uint16_t address) {
  // first 2KiB
  if (address < 0x0800) {
    // printf("DEBUG RAM address: 0x%04X = 0x%02X\n", idx, ram[idx]);
    return ram[address];
  } else if (address < 0x1000) {
    uint16_t normalizedIdx = address - 0x800;
    printf("DEBUG 1st RAM mirror address: 0x%02X -> 0x%02X\n", address,
           normalizedIdx);
    return ram[normalizedIdx];
  } else if (address < 0x1800) {
    uint16_t normalizedIdx = address - 0x1000;
    printf("DEBUG 2nd RAM mirror address: 0x%02X -> 0x%02X\n", address,
           normalizedIdx);
    return ram[normalizedIdx];
  } else if (address < 0x2000) {
    uint16_t normalizedIdx = address - 0x1800;
    printf("DEBUG 3nd RAM mirror address: 0x%02X -> 0x%02X\n", address,
           normalizedIdx);
    return ram[normalizedIdx];
  } else if (address < 0x2008) {
    uint8_t offset = address - 0x2000;
    printf("DEBUG PPU register: %d = 0x%02X\n", offset, ppuRegisters[offset]);
    return ppuRegisters[offset];
  } else if (address < 0x4000) {
    throw "TODO implement PPU register repeats";
  } else if (address < 0x4018) {
    uint8_t offset = address - 0x4000;
    printf("DEBUG APU or I/O register: %d = 0x%02X\n", address,
           apuAndIoRegisters[offset]);
    return apuAndIoRegisters[offset];
  } else if (address < 0x4020) {
    throw "TODO: implement APU & I/O functionality that is normally disabled";
  } else if (address <= 0xFFFF) {
    // mapper
    return mapper->peek16(address);
  }
  throw "Unreachable";
}

Instructions::Instruction VM::decodeInstruction() {
  using enum Instructions::OpCode;
  Instructions::OpCode zero = (Instructions::OpCode)peek16(PC);
  switch (zero) {
  case AND_ABS:
  case LDA_ABS:
  case JMP_ABS:
  case JSR_ABS:
  case STA_ABS:
  case STX_ABS:
  case STY_ABS:
  case LDA_ABS_X:
    PC += 3;
    return Instructions::Instruction{
        .opCode = (Instructions::OpCode)zero,
        .operand =
            {
                .absolute =
                    Address::Absolute{
                        .low = peek16(PC + 1),
                        .high = peek16(PC + 2),
                    },
            },
    };
  case BCC_REL:
  case BCS_REL:
  case BEQ_REL:
  case BNE_REL:
  case BPL_REL:
    PC += 2;
    return Instructions::Instruction{
        .opCode = (Instructions::OpCode)zero,
        .operand = {.relative = peek16(PC + 1)},
    };
  case ASL_A:
  case LSR_A:
    PC += 1;
    return Instructions::Instruction{
        .opCode = (Instructions::OpCode)zero,
        .operand = {.accumulator = nullptr},
    };
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
    PC += 1;
    return Instructions::Instruction{
        .opCode = (Instructions::OpCode)zero,
        .operand = {.implied = nullptr},
    };
  case JMP_INDIRECT:
    PC += 3;
    return Instructions::Instruction{
        .opCode = (Instructions::OpCode)zero,
        .operand =
            {
                .indirect =
                    Address::Absolute{
                        .low = peek16(PC + 1),
                        .high = peek16(PC + 2),
                    },
            },
    };
  case CMP_IMM:
  case CPX_IMM:
  case LDA_IMM:
  case LDX_IMM:
  case LDY_IMM:
    PC += 2;
    return Instructions::Instruction{
        .opCode = (Instructions::OpCode)zero,
        .operand = {.immediate = peek16(PC + 1)},
    };
  case DEC_ZERO:
  case INC_ZERO:
  case LDA_ZERO:
  case STA_ZERO:
    PC += 2;
    return Instructions::Instruction{
        .opCode = (Instructions::OpCode)zero,
        .operand = {.zeropage = peek16(PC + 1)},
    };
  default:
    // This is never freed
    char *msg = new char[256];
    snprintf(msg, 256, "Unimplemented instruction 0x%02X at 0x%04X",
             (uint8_t)zero, PC);
    throw msg;
  }
}

void inline VM::_setN(uint8_t other) { S = (S & _NNot) | (_N & other); }

void inline VM::_setZ(uint8_t other) {
  if (other) {
    // not zero
    S = S & _ZNot;
  } else {
    // is zero
    S = (S & _ZNot) | (_Z);
  }
}

void inline VM::_setC(bool didCarry) {
  uint8_t updateMask = didCarry ? _C : 0x0;
  S = (S & _CNot) | updateMask;
}

void VM::execute(Instructions::Instruction instruction) {
  switch (instruction.opCode) {
    using enum Instructions::OpCode;
    uint8_t value;
  case AND_ABS:
    value = peek(instruction.operand.absolute);
    _setN(value);
    _setZ(value);
    A += value;
    break;
  case ASL_A:
    value = A;
    _setN(value);
    _setZ(value);
    _setC(value & (1 << 7) ? true : false);
    A = value << 1;
    break;
  case BPL_REL:
    // if not negative...
    if ((S & _N) == 0) {
      // TODO: is the operand signed?
      PC += instruction.operand.relative;
    }
    break;
  case JMP_ABS:
    PC = peek(instruction.operand.absolute);
    break;
  case LDA_IMM:
    value = instruction.operand.immediate;
    _setN(value);
    _setZ(value);
    A = value;
    break;
  case LDX_IMM:
    value = instruction.operand.immediate;
    _setN(value);
    _setZ(value);
    X = value;
    break;
  case LDY_IMM:
    value = instruction.operand.immediate;
    _setN(value);
    _setZ(value);
    Y = value;
    break;
  }
}

} // namespace VM
