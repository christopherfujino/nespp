#include <cstring>
#include <stdio.h>

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

    pc = addressLow | (addressHigh << 8);
  }

  Instructions::Instruction current;
  while (1) {
    current = decodeInstruction(pc);
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

Instructions::Instruction VM::decodeInstruction(uint16_t address) {
  uint8_t zero = peek16(address);
  switch (zero) {
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

} // namespace VM
