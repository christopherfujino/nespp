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

/// Negative bitmask
static constexpr uint8_t _N = 1 << 7;
static constexpr uint8_t _NNot = (uint8_t)(~_N);

/// Overflow bitmask
// const uint8_t _V = 1 << 6;

static constexpr uint8_t _D = 1 << 3;
static constexpr uint8_t _DNot = ~_D;

// Interrupt bitmask
static constexpr uint8_t _I = 1 << 2;

/// Zero bitmask
static constexpr uint8_t _Z = 1 << 1;
static constexpr uint8_t _ZNot = ~_Z;

/// Carry bitmask
static constexpr uint8_t _C = 1 << 0;
static constexpr uint8_t _CNot = ~_C;

// Methods
static inline void _setN(VM *vm, uint8_t other) {
  vm->S = (vm->S & _NNot) | (_N & other);
}

static inline void _setZ(VM *vm, uint8_t other) {
  if (other == 0x0) {
    // is zero
    vm->S = (vm->S & _ZNot) | (_Z);
  } else {
    // not zero
    vm->S = vm->S & _ZNot;
  }
}

static inline bool _getZ(VM *vm) { return (vm->S & _Z) > 0; }

static inline void _setC(VM *vm, bool didCarry) {
  uint8_t updateMask = didCarry ? _C : 0x0;
  vm->S = (vm->S & _CNot) | updateMask;
}

inline bool _getZ();
static inline bool _getC(VM *vm) { return (vm->S & _C) > 0; }

static inline void _push(VM *vm, uint8_t v) {
  vmPoke16(vm, 0x0100 + vm->SP, v);
  // I *think* this behaves identically to 6502 wrapping since SP is unsigned
  vm->SP -= 1;
}

static inline void _pushWord(VM *vm, Word word) {
  _push(vm, word.high);
  _push(vm, word.low);
}

static inline uint8_t _pop(VM *vm) {
  vm->SP += 1;
  uint16_t i = 0x0100 + vm->SP;
  assert(i <= 0x01FF && i >= 0x0100);
  return vmPeek16(vm, i);
}

[[maybe_unused]]
static inline Word _popWord(VM *vm) {
  auto low = _pop(vm);
  auto high = _pop(vm);
  return Word{
      .high = high,
      .low = low,
  };
}

static uint8_t _operandToValue(VM *vm, Instruction instruction) {
  using enum AddressingMode;
  switch (instruction.opCode.addressing) {
  case accumulator:
    return vm->A;
  case immediate:
    return instruction.operand.immediate;
  case zeropage:
    return vmPeek8(vm, instruction.operand.zeropage);
  case absolute:
    return vmPeek(vm, instruction.operand.absolute);

  case relative: // This is a branch target
  case implied:  // no operand
  case indirect: // This is a jump target
    throw "Unreachable";
  }
  assert(false);
  throw "Unreachable";
}

static Word _operandToAddress(VM *vm, Instruction instruction) {
  using enum AddressingMode;
  switch (instruction.opCode.addressing) {
  case absolute:
    return instruction.operand.absolute;
  case indirect:
    return Word{
        .high = vmPeek(vm, wordPlus(&instruction.operand.indirect, 1)),
        .low = vmPeek(vm, instruction.operand.indirect),
    };
  case relative:
    // This is an offset from the PC
    // Must cast to signed byte
    return wordPlus(&vm->PC, (int8_t)instruction.operand.relative);
  case zeropage:
    // Full address is this cast to 16-bits
    return Word{
        .high = 0x00,
        .low = instruction.operand.zeropage,
    };
  case accumulator:
  case immediate:
  case implied:
    throw "Unreachable";
  }
  assert(false);
  throw "Unreachable";
}

// TODO: avoid the copy in return.
Mapper0 mapper0New(Rom *_rom) {
  Mapper0 mapper;
  mapper.rom = _rom;

  // TODO: should we copy, or should this just be a light view into the ROM?
  switch (_rom->prgSize) {
  case 0x4000: // 16KiB
    memcpy(mapper.prg, _rom->prgBlob, 0x4000);
    // Mirror
    memcpy(mapper.prg + 0x4000, _rom->prgBlob, 0x4000);
    break;
  case 0x8000: // 32KiB
    memcpy(mapper.prg, _rom->prgBlob, 0x8000);
    break;
  default:
    throw std::runtime_error(
        std::format("Unknown PRG size {:4X}", _rom->prgSize));
  }
  return mapper;
}

uint8_t mapper0Peek16(Mapper0 *mapper, uint16_t address) {
  if (address < 0x6000) {
    throw "Unreachable";
  } else if (address < 0x8000) {
    // unbanked PRG-RAM
    throw "TODO: implement PRG-RAM";
  } else {
    // either continuation of PRG or mirror
    uint16_t offset = address - 0x8000;
    return mapper->prg[offset];
  }
}

void mapper0Poke16(Mapper0 *mapper, uint16_t address, uint8_t value) {
  if (address < 0x6000) {
    throw "Unreachable";
  } else if (address < 0x8000) {
    // unbanked PRG-RAM
    throw "TODO: implement PRG-RAM";
  } else {
    // either continuation of PRG or mirror
    uint16_t offset = address - 0x8000;
    mapper->prg[offset] = value;
  }
}

uint8_t mapperPeek16(Mapper *mapper, uint16_t address) {
  switch (mapper->tag) {
  case mapperTag0:
    return mapper0Peek16(&mapper->mapper0, address);
  }
  // unreachable
  abort();
}

void mapperPoke16(Mapper *mapper, uint16_t address, uint8_t value) {
  switch (mapper->tag) {
    case mapperTag0:
      return mapper0Poke16(&mapper->mapper0, address, value);
  }
  abort();
}

VM vmNew(Rom *_rom, void(*debug)(VM *, std::string)) {
  switch (_rom->mapper) {
  case 0:
    return VM{
        .PC = Word{},
        .A = 0,
        .X = 0,
        .Y = 0,
        .SP = 0xFF,
        .S = 1 << 5,
        .ram = {0},
        .ppuRegisters = {0},
        .apuAndIoRegisters = {0},
        .mapper =
            Mapper{
                .tag = mapperTag0,
                .mapper0 = mapper0New(_rom),
            },
        .rom = std::move(_rom),
        .debug = debug,
    };
  default:
    throw "Oops!";
  }
}

void VMStart(VM *vm) {
  constexpr uint16_t startingLowAddress = 0xFFFC;
  constexpr uint16_t startingHighAddress = 0xFFFD;

  vm->PC = {
      .high = vmPeek16(vm, startingHighAddress),
      .low = vmPeek16(vm, startingLowAddress),
  };

  Instruction current;
  while (1) {
    current = vmDecodeInstruction(vm);
    vmExecute(vm, current);
  }
}

uint8_t vmPeek(VM *vm, Word address) {
  return vmPeek16(vm, address.low | (address.high << 8));
}

uint8_t vmPeek8(VM *vm, uint8_t offset) { return vm->ram[offset]; }

uint8_t vmPeek16(VM *vm, uint16_t address) {
  // first 2KiB
  if (address < 0x0800) {
    // printf("DEBUG RAM address: 0x%04X = 0x%02X\n", idx, ram[idx]);
    return vm->ram[address];
  } else if (address < 0x1000) {
    uint16_t normalizedIdx = address - 0x800;
    // printf("DEBUG 1st RAM mirror address: 0x%02X -> 0x%02X\n", address,
    //        normalizedIdx);
    return vm->ram[normalizedIdx];
  } else if (address < 0x1800) {
    uint16_t normalizedIdx = address - 0x1000;
    // printf("DEBUG 2nd RAM mirror address: 0x%02X -> 0x%02X\n", address,
    //        normalizedIdx);
    return vm->ram[normalizedIdx];
  } else if (address < 0x2000) {
    uint16_t normalizedIdx = address - 0x1800;
    // printf("DEBUG 3nd RAM mirror address: 0x%02X -> 0x%02X\n", address,
    //        normalizedIdx);
    return vm->ram[normalizedIdx];
  } else if (address < 0x2008) {
    uint8_t offset = address - 0x2000;
    vm->debug(vm, std::format("DEBUG PPU register: {} = 0x{:02X}", offset,
                      vm->ppuRegisters[offset]));
    return vm->ppuRegisters[offset];
  } else if (address < 0x4000) {
    throw "TODO implement PPU register repeats";
  } else if (address < 0x4018) {
    uint8_t offset = address - 0x4000;
    vm->debug(vm, std::format("DEBUG APU or I/O register: {} = 0x{:02X}", address,
                      vm->apuAndIoRegisters[offset]));
    return vm->apuAndIoRegisters[offset];
  } else if (address < 0x4020) {
    throw "TODO: implement APU & I/O functionality that is normally disabled";
  } else if (address <= 0xFFFF) {
    // mapper
    return mapperPeek16(&vm->mapper, address);
  }
  throw "Unreachable";
}

void vmPoke(VM *vm, Word address, uint8_t value) {
  vmPoke16(vm, address.low | (address.high << 8), value);
}

void vmPoke16(VM *vm, uint16_t address, uint8_t value) {
  // first 2KiB
  if (address < 0x0800) {
    // printf("DEBUG RAM address: 0x%04X = 0x%02X\n", idx, ram[idx]);
    vm->ram[address] = value;
  } else if (address < 0x1000) {
    uint16_t normalizedIdx = address - 0x800;
    vm->ram[normalizedIdx] = value;
  } else if (address < 0x1800) {
    uint16_t normalizedIdx = address - 0x1000;
    vm->ram[normalizedIdx] = value;
  } else if (address < 0x2000) {
    uint16_t normalizedIdx = address - 0x1800;
    vm->ram[normalizedIdx] = value;
  } else if (address < 0x2008) {
    uint8_t offset = address - 0x2000;
    vm->ppuRegisters[offset] = value;
  } else if (address < 0x4000) {
    throw "TODO implement PPU register repeats";
  } else if (address < 0x4018) {
    uint8_t offset = address - 0x4000;
    vm->apuAndIoRegisters[offset] = value;
  } else if (address < 0x4020) {
    throw "TODO: implement APU & I/O functionality that is normally disabled";
  } else if (address <= 0xFFFF) {
    // mapper
    mapperPoke16(&vm->mapper, address, value);
  } else {
    throw std::runtime_error(std::format("Invalid address 0x{:4X}", address));
  }
}

Instruction vmDecodeInstruction(VM *vm) {
  Instruction instruction;
  uint8_t _rawCode = vmPeek(vm, vm->PC); // for debugging
  OpCode code = opCodeLookup[_rawCode];
  if (code.type == unimplemented) {
    throw std::runtime_error(
        std::format("Unimplemented instruction 0x{:02X} at 0x{:04X}", _rawCode,
                    wordTo16(&vm->PC)));
  }
  switch (code.addressing) {
  case AddressingMode::absolute:
    instruction = Instruction{
        code,
        InstructionOperandUnion{
            .absolute =
                Word{
                    .high = vmPeek(vm, wordPlus(&vm->PC, 2)),
                    .low = vmPeek(vm, wordPlus(&vm->PC, 1)),
                },
        },
    };
    vm->PC = wordPlus(&vm->PC, 3);
    break;
  case AddressingMode::relative:
    instruction = Instruction{
        code,
        InstructionOperandUnion{
            .relative = vmPeek(vm, wordPlus(&vm->PC, 1)),
        },
    };
    vm->PC = wordPlus(&vm->PC, 2);
    break;
  case AddressingMode::accumulator:
    instruction = Instruction{
        code,
        InstructionOperandUnion{.accumulator = nullptr},
    };
    vm->PC = wordPlus(&vm->PC, 1);
    break;
  case AddressingMode::implied:
    instruction = Instruction{
        code,
        InstructionOperandUnion{.implied = nullptr},
    };
    vm->PC = wordPlus(&vm->PC, 1);
    break;
  case AddressingMode::indirect:
    instruction = Instruction{
        code,
        InstructionOperandUnion{
            .indirect =
                Word{
                    .high = vmPeek(vm, wordPlus(&vm->PC, 2)),
                    .low = vmPeek(vm, wordPlus(&vm->PC, 1)),
                },
        },
    };
    vm->PC = wordPlus(&vm->PC, 3);
    break;
  case AddressingMode::immediate:
    instruction = {
        code,
        {.immediate = vmPeek(vm, wordPlus(&vm->PC, 1))},
    };
    vm->PC = wordPlus(&vm->PC, 2);
    break;
  case AddressingMode::zeropage:
    instruction = {
        code,
        {.zeropage = vmPeek(vm, wordPlus(&vm->PC, 1))},
    };
    vm->PC = wordPlus(&vm->PC, 2);
    break;
  default:
    throw std::runtime_error(
        std::format("Unimplemented instruction 0x{:02X} at 0x{:04X}", _rawCode,
                    wordTo16(&vm->PC)));
  }
  return instruction;
}

void vmExecute(VM *vm, Instruction instruction) {
  Word address;
  switch (instruction.opCode.type) {
    using enum OpCodeType;
    uint8_t value;
  case AND:
    address = _operandToAddress(vm, instruction);
    value = vmPeek(vm, address);
    // TODO: Should this be here?
    _setN(vm, value);
    _setZ(vm, value);
    vm->A = vm->A & value;
    return;
  case ASL:
    value = _operandToValue(vm, instruction);
    _setN(vm, value);
    _setZ(vm, value);
    _setC(vm, value & (1 << 7) ? true : false);
    vm->A = value << 1;
    return;
  case BCC:
    if (!_getC(vm)) {
      vm->PC = _operandToAddress(vm, instruction);
      vm->debug(vm, std::format("Jumping to ${:04X}", wordTo16(&vm->PC)));
    }
    return;
  case BCS:
    if (_getC(vm)) {
      vm->PC = _operandToAddress(vm, instruction);
      vm->debug(vm, std::format("Jumping to ${:04X}", wordTo16(&vm->PC)));
    }
    return;
  case BEQ:
    if (_getZ(vm)) {
      vm->PC = _operandToAddress(vm, instruction);
      vm->debug(vm, std::format("Jumping to ${:04X}", wordTo16(&vm->PC)));
    }
    return;
  case BNE:
    if (!_getZ(vm)) {
      vm->PC = _operandToAddress(vm, instruction);
      vm->debug(vm, std::format("Jumping to ${:04X}", wordTo16(&vm->PC)));
    }
    return;
  case BPL:
    // if not negative...
    if ((vm->S & _N) == 0) {
      vm->PC = _operandToAddress(vm, instruction);
      vm->debug(vm, std::format("Jumping to ${:04X}", wordTo16(&vm->PC)));
    }
    return;
  case CLD:
    vm->S &= _DNot;
    return;
  case CMP:
    if (instruction.opCode.addressing == AddressingMode::immediate) {
      value = instruction.operand.immediate;
    } else {
      address = _operandToAddress(vm, instruction);
      value = vmPeek(vm, address);
    }
    value = vm->A - value;
    _setC(vm, value);
    _setZ(vm, value);
    _setN(vm, value);
    return;
  case CPX:
    if (instruction.opCode.addressing == AddressingMode::immediate) {
      value = instruction.operand.immediate;
    } else {
      address = _operandToAddress(vm, instruction);
      value = vmPeek(vm, address);
    }
    value = vm->X - value;
    _setC(vm, value);
    _setZ(vm, value);
    _setN(vm, value);
    return;
  case CPY:
    if (instruction.opCode.addressing == AddressingMode::immediate) {
      value = instruction.operand.immediate;
    } else {
      address = _operandToAddress(vm, instruction);
      value = vmPeek(vm, address);
    }
    value = vm->Y - value;
    _setC(vm, value);
    _setZ(vm, value);
    _setN(vm, value);
    return;
  case DEC:
    address = _operandToAddress(vm, instruction);
    value = vmPeek(vm, address) - 1;
    vmPoke(vm, address, value);
    _setZ(vm, value);
    _setN(vm, value);
    return;
  case DEX:
    vm->X -= 1;
    // Is this handled correctly even though X is unsigned?
    _setN(vm, vm->X);
    _setZ(vm, vm->X);
    return;
  case DEY:
    vm->Y -= 1;
    // Is this handled correctly even though X is unsigned?
    _setN(vm, vm->Y);
    _setZ(vm, vm->Y);
    return;
  case INC:
    address = _operandToAddress(vm, instruction);
    value = vmPeek(vm, address) + 1;
    vmPoke(vm, address, value);
    _setN(vm, value);
    _setZ(vm, value);
    return;
  case INX:
    vm->X += 1;
    _setN(vm, vm->X);
    _setZ(vm, vm->X);
    return;
  case INY:
    vm->Y += 1;
    _setN(vm, vm->Y);
    _setZ(vm, vm->Y);
    return;
  case JMP:
    vm->PC = _operandToAddress(vm, instruction);
    vm->debug(vm, std::format("Jumping to ${:04X}", wordTo16(&vm->PC)));
    return;
  case JSR:
    // https://retrocomputing.stackexchange.com/questions/19543/why-does-the-6502-jsr-instruction-only-increment-the-return-address-by-2-bytes
    _pushWord(vm, wordMinus(&vm->PC, 1));
    vm->PC = _operandToAddress(vm, instruction);
    vm->debug(vm, std::format("Jumping to ${:04X}", wordTo16(&vm->PC)));
    return;
  case LDA:
    // TODO: handle carry with ABS,X?
    value = _operandToValue(vm, instruction);
    _setN(vm, value);
    _setZ(vm, value);
    vm->A = value;
    return;
  case LDX:
    value = _operandToValue(vm, instruction);
    _setN(vm, value);
    _setZ(vm, value);
    vm->X = value;
    return;
  case LDY:
    value = _operandToValue(vm, instruction);
    _setN(vm, value);
    _setZ(vm, value);
    vm->Y = value;
    return;
  case LSR:
    // Will shift right-most bit into C
    if (instruction.opCode.addressing == AddressingMode::accumulator) {
      value = vm->A;
      _setC(vm, (value & 0x1) > 0);
      value = value >> 1;
      vm->A = value;
    } else {
      address = _operandToAddress(vm, instruction);
      value = vmPeek(vm, address);
      _setC(vm, (value & 0x1) > 0);
      value = value >> 1;
      vmPoke(vm, address, value);
    }
    _setZ(vm, value);
    _setN(vm, 0);
    return;
  case PHA:
    _push(vm, vm->A);
    return;
  case RTS:
    // See JSR
    vm->PC = wordPlus(&vm->PC, 1);
    return;
  case SEI:
    vm->S |= _I;
    return;
  case STA:
    address = _operandToAddress(vm, instruction);
    vmPoke(vm, address, vm->A);
    return;
  case STX:
    address = _operandToAddress(vm, instruction);
    vmPoke(vm, address, vm->X);
    return;
  case STY:
    address = _operandToAddress(vm, instruction);
    vmPoke(vm, address, vm->Y);
    return;
  case TAX:
    vm->X = vm->A;
    _setN(vm, vm->X);
    _setZ(vm, vm->X);
    return;
  case TXS:
    vm->SP = vm->X;
    return;
  case unimplemented:
    abort();
    // throw std::runtime_error(
    //     std::string("Tried to execute unimplemented instruction: ") +
    //     instruction.opCode.toString());
  }
}
