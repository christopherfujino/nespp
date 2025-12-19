#include "../include/vm.h"           // for VM, Mapper0, Mapper
#include "../include/instructions.h" // for OpCode, Instruction, OpCode::AND_ABS, OpC...
#include "../include/rom.h"          // for Rom
#include "../include/word.h"         // for Absolute
#include <array>
#include <cassert>
#include <cstring>   // for memcpy
#include <format>    // std::format
#include <stdexcept> // std::runtime_except
#include <utility>   // for std::move

namespace NESPP {

Mapper0::Mapper0(std::shared_ptr<Rom> _rom) {
  this->rom = std::move(_rom);

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
  } else {
    // either continuation of PRG or mirror
    uint16_t offset = address - 0x8000;
    return prg[offset];
  }
}

void Mapper0::poke16(uint16_t address, uint8_t value) {
  if (address < 0x6000) {
    throw "Unreachable";
  } else if (address < 0x8000) {
    // unbanked PRG-RAM
    throw "TODO: implement PRG-RAM";
  } else {
    // either continuation of PRG or mirror
    uint16_t offset = address - 0x8000;
    prg[offset] = value;
  }
}

VM::VM(std::shared_ptr<Rom> _rom) {
  this->rom = std::move(_rom);

  switch (rom->mapper) {
  case 0:
    // copy shared_ptr
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

  Instruction current;
  while (1) {
    // printf("$%02X: ", PC);
    current = decodeInstruction();
    execute(current);
  }
}

uint8_t VM::peek(Word address) {
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

void VM::poke(Word address, uint8_t value) {
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

Instruction VM::decodeInstruction() {
  Instruction instruction;
  uint8_t _rawCode = peek(PC); // for debugging
  OpCode code = opCodeLookup[_rawCode];
  switch (code.addressing) {
  case AddressingMode::absolute:
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
  case AddressingMode::relative:
    instruction = {
        code,
        {.relative = peek(PC + 1)},
    };
    PC += 2;
    break;
  case AddressingMode::accumulator:
    instruction = {
        code,
        {.accumulator = nullptr},
    };
    PC += 1;
    break;
  case AddressingMode::implied:
    instruction = {
        code,
        {.implied = nullptr},
    };
    PC += 1;
    break;
  case AddressingMode::indirect:
    instruction = {
        code,
        // TODO is this right?!
        {.indirect = {peek(PC + 2), peek(PC + 1)}},
    };
    PC += 3;
    break;
  case AddressingMode::immediate:
    instruction = {
        code,
        {.immediate = peek(PC + 1)},
    };
    PC += 2;
    break;
  case AddressingMode::zeropage:
    instruction = {
        code,
        {.zeropage = peek(PC + 1)},
    };
    PC += 2;
    break;
  default:
    throw std::runtime_error(std::format(
        "Unimplemented instruction 0x{:02X} at 0x{:04X}", _rawCode, PC.to16()));
  }

  return instruction;
}

inline void VM::_setN(uint8_t other) { S = (S & _NNot) | (_N & other); }

inline void VM::_setZ(uint8_t other) {
  if (other == 0x0) {
    // is zero
    S = (S & _ZNot) | (_Z);
  } else {
    // not zero
    S = S & _ZNot;
  }
}

inline bool VM::_getZ() { return ((S & _Z) > 0); }

inline void VM::_setC(bool didCarry) {
  uint8_t updateMask = didCarry ? _C : 0x0;
  S = (S & _CNot) | updateMask;
}

inline bool VM::_getC() { return ((S & _C) > 0); }

void VM::_push(uint8_t v) {
  poke16(0x0100 + SP, v);
  // I *think* this behaves identically to 6502 wrapping since SP is unsigned
  SP -= 1;
}

void VM::_pushWord(Word word) {
  _push(word.high);
  _push(word.low);
}

uint8_t VM::_pop() {
  SP += 1;
  uint16_t i = 0x0100 + SP;
  assert(i <= 0x01FF && i >= 0x0100);
  return peek16(i);
}

Word VM::_popWord() {
  auto low = _pop();
  auto high = _pop();
  return {high, low};
}

void VM::execute(Instruction instruction) {
  Word address;
  switch (instruction.opCode.type) {
    using enum OpCodeType;
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
  case BCC:
    if (!_getC()) {
      PC = _operandToAddress(instruction);
      debug(std::format("Jumping to ${:04X}", PC.to16()));
    }
    return;
  case BCS:
    if (_getC()) {
      PC = _operandToAddress(instruction);
      debug(std::format("Jumping to ${:04X}", PC.to16()));
    }
    return;
  case BEQ:
    if (_getZ()) {
      PC = _operandToAddress(instruction);
      debug(std::format("Jumping to ${:04X}", PC.to16()));
    }
    return;
  case BNE:
    if (!_getZ()) {
      PC = _operandToAddress(instruction);
      debug(std::format("Jumping to ${:04X}", PC.to16()));
    }
    return;
  case BPL:
    // if not negative...
    if ((S & _N) == 0) {
      PC = _operandToAddress(instruction);
      debug(std::format("Jumping to ${:04X}", PC.to16()));
    }
    return;
  case CLD:
    S &= _DNot;
    return;
  case CMP:
    if (instruction.opCode.addressing == AddressingMode::immediate) {
      value = instruction.operand.immediate;
    } else {
      address = _operandToAddress(instruction);
      value = peek(address);
    }
    value = A - value;
    _setC(value);
    _setZ(value);
    _setN(value);
    return;
  case CPX:
    if (instruction.opCode.addressing == AddressingMode::immediate) {
      value = instruction.operand.immediate;
    } else {
      address = _operandToAddress(instruction);
      value = peek(address);
    }
    value = X - value;
    _setC(value);
    _setZ(value);
    _setN(value);
    return;
  case CPY:
    if (instruction.opCode.addressing == AddressingMode::immediate) {
      value = instruction.operand.immediate;
    } else {
      address = _operandToAddress(instruction);
      value = peek(address);
    }
    value = Y - value;
    _setC(value);
    _setZ(value);
    _setN(value);
    return;
  case DEC:
    address = _operandToAddress(instruction);
    value = peek(address) - 1;
    poke(address, value);
    _setZ(value);
    _setN(value);
    return;
  case DEX:
    X -= 1;
    // Is this handled correctly even though X is unsigned?
    _setN(X);
    _setZ(X);
    return;
  case DEY:
    Y -= 1;
    // Is this handled correctly even though X is unsigned?
    _setN(Y);
    _setZ(Y);
    return;
  case INC:
    address = _operandToAddress(instruction);
    value = peek(address) + 1;
    poke(address, value);
    _setN(value);
    _setZ(value);
    return;
  case INX:
    X += 1;
    _setN(X);
    _setZ(X);
    return;
  case INY:
    Y += 1;
    _setN(Y);
    _setZ(Y);
    return;
  case JMP:
    PC = _operandToAddress(instruction);
    debug(std::format("Jumping to ${:04X}", PC.to16()));
    return;
  case JSR:
    // https://retrocomputing.stackexchange.com/questions/19543/why-does-the-6502-jsr-instruction-only-increment-the-return-address-by-2-bytes
    _pushWord(PC - 1);
    PC = _operandToAddress(instruction);
    debug(std::format("Jumping to ${:04X}", PC.to16()));
    return;
  case LDA:
    // TODO: handle carry with ABS,X?
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
  case LSR:
    // Will shift right-most bit into C
    if (instruction.opCode.addressing == AddressingMode::accumulator) {
      value = A;
      _setC((value & 0x1) > 0);
      value = value >> 1;
      A = value;
    } else {
      address = _operandToAddress(instruction);
      value = peek(address);
      _setC((value & 0x1) > 0);
      value = value >> 1;
      poke(address, value);
    }
    _setZ(value);
    _setN(0);
    return;
  case PHA:
    _push(A);
    return;
  case RTS:
    // See JSR
    PC = _popWord() + 1;
    return;
  case SEI:
    S |= _I;
    return;
  case STA:
    address = _operandToAddress(instruction);
    poke(address, A);
    return;
  case STX:
    address = _operandToAddress(instruction);
    poke(address, X);
    return;
  case STY:
    address = _operandToAddress(instruction);
    poke(address, Y);
    return;
  case TAX:
    X = A;
    _setN(X);
    _setZ(X);
    return;
  case TXS:
    SP = X;
    return;
  case unimplemented:
    throw std::runtime_error("Tried to execute unimplemented instruction");
  }
}

uint8_t VM::_operandToValue(Instruction instruction) {
  using enum AddressingMode;
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

Word VM::_operandToAddress(Instruction instruction) {
  using enum AddressingMode;
  switch (instruction.opCode.addressing) {
  case absolute:
    return instruction.operand.absolute;
  case indirect:
    return Word{
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
    return Word(0x0, instruction.operand.zeropage);
  case accumulator:
  case immediate:
  case implied:
    throw "Unreachable";
  }
  assert(false);
  throw "Unreachable";
}

} // namespace NESPP
