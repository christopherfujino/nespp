#include "../include/vm.h"      // for VM, Mapper0, Mapper
#include "../include/address.h" // for Absolute
#include "../include/instructions.h" // for OpCode, Instruction, OpCode::AND_ABS, OpC...
#include "../include/rom.h"          // for Rom
#include <cstring>                   // for memcpy
#include <format>                    // std::format
#include <stdexcept>                 // std::runtime_except
#include <stdint.h>                  // for uint8_t, uint16_t, int8_t

namespace NESPP {

namespace VM {

Mapper0::Mapper0(Rom *rom) {
  this->rom = rom;

  // TODO: should we copy, or should this just be a light view into the ROM?
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
    throw std::runtime_error(
        std::format("Unknown PRG size {:4X}", rom->prgSize));
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

void Mapper0::poke16(uint16_t address, uint8_t value) {
  if (address < 0x6000) {
    throw "Unreachable";
  } else if (address < 0x8000) {
    // unbanked PRG-RAM
    throw "TODO: implement PRG-RAM";
  } else if (address <= 0xFFFF) {
    // either continuation of PRG or mirror
    uint16_t offset = address - 0x8000;
    prg[offset] = value;
  } else {
    throw "Unreachable";
  }
}

VM::VM(Rom *rom) {
  this->rom = rom;

  switch (rom->mapper) {
  case 0:
    mapper = new Mapper0(rom);
    break;
  default:
    throw "Oops!";
  }
}

VM::~VM() { delete mapper; }

void VM::start() {
  {
    uint8_t low = peek16(0xFFFC);
    uint8_t high = peek16(0xFFFD);

    PC = {high, low};
  }

  Instructions::Instruction current;
  while (1) {
    // printf("$%02X: ", PC);
    current = decodeInstruction();
    execute(current);
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
    // printf("DEBUG 1st RAM mirror address: 0x%02X -> 0x%02X\n", address,
    //        normalizedIdx);
    return ram[normalizedIdx];
  } else if (address < 0x1800) {
    uint16_t normalizedIdx = address - 0x1000;
    // printf("DEBUG 2nd RAM mirror address: 0x%02X -> 0x%02X\n", address,
    //        normalizedIdx);
    return ram[normalizedIdx];
  } else if (address < 0x2000) {
    uint16_t normalizedIdx = address - 0x1800;
    // printf("DEBUG 3nd RAM mirror address: 0x%02X -> 0x%02X\n", address,
    //        normalizedIdx);
    return ram[normalizedIdx];
  } else if (address < 0x2008) {
    uint8_t offset = address - 0x2000;
    debug(std::format("DEBUG PPU register: {} = 0x{:02X}", offset,
                      ppuRegisters[offset]));
    return ppuRegisters[offset];
  } else if (address < 0x4000) {
    throw "TODO implement PPU register repeats";
  } else if (address < 0x4018) {
    uint8_t offset = address - 0x4000;
    debug(std::format("DEBUG APU or I/O register: {} = 0x{:02X}", address,
                      apuAndIoRegisters[offset]));
    return apuAndIoRegisters[offset];
  } else if (address < 0x4020) {
    throw "TODO: implement APU & I/O functionality that is normally disabled";
  } else if (address <= 0xFFFF) {
    // mapper
    return mapper->peek16(address);
  }
  throw "Unreachable";
}

void VM::poke(Address::Absolute address, uint8_t value) {
  poke16(address.low | (address.high << 8), value);
}

void VM::poke16(uint16_t address, uint8_t value) {
  // first 2KiB
  if (address < 0x0800) {
    // printf("DEBUG RAM address: 0x%04X = 0x%02X\n", idx, ram[idx]);
    ram[address] = value;
  } else if (address < 0x1000) {
    uint16_t normalizedIdx = address - 0x800;
    ram[normalizedIdx] = value;
  } else if (address < 0x1800) {
    uint16_t normalizedIdx = address - 0x1000;
    ram[normalizedIdx] = value;
  } else if (address < 0x2000) {
    uint16_t normalizedIdx = address - 0x1800;
    ram[normalizedIdx] = value;
  } else if (address < 0x2008) {
    uint8_t offset = address - 0x2000;
    ppuRegisters[offset] = value;
  } else if (address < 0x4000) {
    throw "TODO implement PPU register repeats";
  } else if (address < 0x4018) {
    uint8_t offset = address - 0x4000;
    apuAndIoRegisters[offset] = value;
  } else if (address < 0x4020) {
    throw "TODO: implement APU & I/O functionality that is normally disabled";
  } else if (address <= 0xFFFF) {
    // mapper
    mapper->poke16(address, value);
  } else {
    throw std::runtime_error(std::format("Invalid address 0x{:4X}", address));
  }
}

Instructions::Instruction VM::decodeInstruction() {
  Instructions::Instruction instruction;
  uint8_t _rawCode = peek(PC); // for debugging
  Instructions::OpCode code = Instructions::opCodeLookup[_rawCode];
  switch (code.addressing) {
  case Instructions::AddressingMode::absolute:
    instruction = {
        code,
        {.absolute =
             {
                 peek(PC + 2),
                 peek(PC + 1),
             }},
    };
    PC += 3;
    break;
  case Instructions::AddressingMode::relative:
    instruction = {
        code,
        {.relative = peek(PC + 1)},
    };
    PC += 2;
    break;
  case Instructions::AddressingMode::accumulator:
    instruction = {
        code,
        {.accumulator = nullptr},
    };
    PC += 1;
    break;
  case Instructions::AddressingMode::implied:
    instruction = {
        code,
        {.implied = nullptr},
    };
    PC += 1;
    break;
  case Instructions::AddressingMode::indirect:
    instruction = {
        code,
        // TODO is this right?!
        {.indirect = {peek(PC + 2), peek(PC + 1)}},
    };
    PC += 3;
    break;
  case Instructions::AddressingMode::immediate:
    instruction = {
        code,
        {.immediate = peek(PC + 1)},
    };
    PC += 2;
    break;
  case Instructions::AddressingMode::zeropage:
    instruction = {
        code,
        {.zeropage = peek(PC + 1)},
    };
    PC += 2;
    break;
  default:
    throw std::runtime_error(std::format(
        "Unimplemented instruction 0x{:2X} at 0x{:4X}", _rawCode, PC.to16()));
  }

  return instruction;
}

void inline VM::_setN(uint8_t other) { S = (S & _NNot) | (_N & other); }

void inline VM::_setZ(uint8_t other) {
  if (other == 0x0) {
    // is zero
    S = (S & _ZNot) | (_Z);
  } else {
    // not zero
    S = S & _ZNot;
  }
}

void inline VM::_setC(bool didCarry) {
  uint8_t updateMask = didCarry ? _C : 0x0;
  S = (S & _CNot) | updateMask;
}

void VM::execute(Instructions::Instruction instruction) {
  Address::Absolute address;
  switch (instruction.opCode.type) {
    using enum Instructions::OpCodeType;
    uint8_t value;
  case AND:
    address = _operandToAddress(instruction);
    value = peek(address);
    // TODO: Should this be here?
    _setN(value);
    _setZ(value);
    A = A & value;
    return;
  case ASL:
    value = _operandToValue(instruction);
    _setN(value);
    _setZ(value);
    _setC(value & (1 << 7) ? true : false);
    A = value << 1;
    return;
  case BPL:
    // if not negative...
    if ((S & _N) == 0) {
      PC = _operandToAddress(instruction);
      debug(std::format("Jumping to ${:4X}", PC.to16()));
    }
    return;
  case CLD:
    S &= _DNot;
    return;
  case JMP:
    PC = _operandToAddress(instruction);
    debug(std::format("Jumping to ${:4X}", PC.to16()));
    return;
  case LDA:
    value = _operandToValue(instruction);
    _setN(value);
    _setZ(value);
    A = value;
    return;
  case LDX:
    value = _operandToValue(instruction);
    _setN(value);
    _setZ(value);
    X = value;
    return;
  case LDY:
    value = _operandToValue(instruction);
    _setN(value);
    _setZ(value);
    Y = value;
    return;
  case SEI:
    S |= _I;
    return;
  case STA:
    address = _operandToAddress(instruction);
    poke(address, A);
    return;
  case TXS:
    SP = X;
    return;

  default:
    throw std::runtime_error(
        std::format("TODO execute instruction: {}", instruction.toString()));
    // case BNE_REL:
    //   if (_Z == 0) {
    //     PC += instruction.operand.relative;
    //   }
    //   return;
    // case DEX:
    //   X -= 1;
    //   // Is this handled correctly even though X is unsigned?
    //   _setN(X);
    //   _setZ(X);
    //   return;
    // case DEY:
    //   Y -= 1;
    //   // Is this handled correctly even though X is unsigned?
    //   _setN(Y);
    //   _setZ(Y);
    //   return;
    // case LDA_ABS:
    //   value = instruction.operand.absolute.to16();
    //   _setN(value);
    //   _setZ(value);
    //   A = value;
    //   return;
    // case LDA_ABS_X:
    //   // operand is address; effective address is address incremented by X
    //   with
    //   // carry **
    //   // TODO: Do we add _C?!?!??!
    //   address = instruction.operand.absolute.to16() + X;
    //   value = peek16(address);
    //   _setN(value);
    //   _setZ(value);
    //   A = value;
    //   return;
    // case LDA_IMM:
    //   value = instruction.operand.immediate;
    //   _setN(value);
    //   _setZ(value);
    //   A = value;
    //   return;
    // case LDY_IMM:
    //   value = instruction.operand.immediate;
    //   _setN(value);
    //   _setZ(value);
    //   Y = value;
    //   return;
    // case PHA:
    //   poke16(0x0100 + SP, A);
    //   // I *think* this behaves identically to 6502 wrapping since SP is
    //   unsigned
    //   --SP;
    //   return;
    // case STX_ABS:
    //   poke(instruction.operand.absolute, X);
    //   return;
  }
  throw std::runtime_error(
      std::format("TODO: implement instruction {} in `VM::execute()`",
                  instruction.toString()));
}

uint8_t VM::_operandToValue(Instructions::Instruction instruction) {
  using enum Instructions::AddressingMode;
  switch (instruction.opCode.addressing) {
  case accumulator:
    return A;
  case immediate:
    return instruction.operand.immediate;
  case zeropage:
    return peek8(instruction.operand.zeropage);
  case absolute:
    return peek(instruction.operand.absolute);

  case relative: // This is a branch target
  case implied:  // no operand
  case indirect: // This is a jump target
    throw "Unreachable";
  }
  assert(false);
  throw "Unreachable";
}

Address::Absolute VM::_operandToAddress(Instructions::Instruction instruction) {
  using enum Instructions::AddressingMode;
  switch (instruction.opCode.addressing) {
  case absolute:
    return instruction.operand.absolute;
  case indirect:
    return Address::Absolute{
        // high
        peek(instruction.operand.indirect + 1),
        // low
        peek(instruction.operand.indirect),
    };
  case relative:
    // This is an offset from the PC
    return PC + static_cast<int8_t>(instruction.operand.relative);
  case zeropage:
    // Full address is this cast to 16-bits
    return Address::Absolute(0x0, instruction.operand.zeropage);
  case accumulator:
  case immediate:
  case implied:
    throw "Unreachable";
  }
  assert(false);
  throw "Unreachable";
}

} // namespace VM

} // namespace NESPP
