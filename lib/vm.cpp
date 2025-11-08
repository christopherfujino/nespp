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
  uint8_t zero = peek16(PC);
  switch (zero) {
  case Instructions::AND_ABS:
  case Instructions::LDA_ABS:
  case Instructions::JMP_ABS:
  case Instructions::JSR_ABS:
  case Instructions::STA_ABS:
  case Instructions::STX_ABS:
  case Instructions::STY_ABS:
  case Instructions::LDA_ABS_X:
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
  case Instructions::BCC_REL:
  case Instructions::BCS_REL:
  case Instructions::BEQ_REL:
  case Instructions::BNE_REL:
  case Instructions::BPL_REL:
    PC += 2;
    return Instructions::Instruction{
        .opCode = (Instructions::OpCode)zero,
        .operand = {.relative = peek16(PC + 1)},
    };
  case Instructions::ASL_A:
  case Instructions::LSR_A:
    PC += 1;
    return Instructions::Instruction{
        .opCode = (Instructions::OpCode)zero,
        .operand = {.accumulator = nullptr},
    };
  case Instructions::CLD:
  case Instructions::DEX:
  case Instructions::DEY:
  case Instructions::INX:
  case Instructions::INY:
  case Instructions::PHA:
  case Instructions::RTS:
  case Instructions::SEI:
  case Instructions::TAX:
  case Instructions::TXS:
    PC += 1;
    return Instructions::Instruction{
        .opCode = (Instructions::OpCode)zero,
        .operand = {.implied = nullptr},
    };
  case Instructions::JMP_INDIRECT:
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
  case Instructions::CMP_IMM:
  case Instructions::CPX_IMM:
  case Instructions::LDA_IMM:
  case Instructions::LDX_IMM:
  case Instructions::LDY_IMM:
    PC += 2;
    return Instructions::Instruction{
        .opCode = (Instructions::OpCode)zero,
        .operand = {.immediate = peek16(PC + 1)},
    };
  case Instructions::DEC_ZERO:
  case Instructions::INC_ZERO:
  case Instructions::LDA_ZERO:
  case Instructions::STA_ZERO:
    PC += 2;
    return Instructions::Instruction{
        .opCode = (Instructions::OpCode)zero,
        .operand = {.zeropage = peek16(PC + 1)},
    };
  default:
    // This is never freed
    char *msg = new char[256];
    snprintf(msg, 256, "Unimplemented instruction 0x%02X at 0x%04X", zero, PC);
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
    uint8_t value;
  case Instructions::AND_ABS:
    value = peek(instruction.operand.absolute);
    _setN(value);
    _setZ(value);
    A += value;
    return;
  case Instructions::ASL_A:
    value = A;
    _setN(value);
    _setZ(value);
    _setC(value & (1 << 7) ? true : false);
    A = value << 1;
    return;
  case Instructions::BPL_REL:
    // if not negative...
    if ((S & _N) == 0) {
      // TODO: is the operand signed?
      PC += instruction.operand.relative;
    }
    return;
  case Instructions::JMP_ABS:
    PC = peek(instruction.operand.absolute);
    return;
  }
}

} // namespace VM
