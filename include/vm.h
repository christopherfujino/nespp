#pragma once

#include <cstdint>

#include "address.h"
#include "instructions.h"
#include "rom.h"

namespace VM {

class Mapper {
public:
  virtual ~Mapper() {}
  virtual uint8_t peek16(uint16_t address) = 0;
};

class Mapper0 : public Mapper {
public:
  Mapper0(Rom *rom);
  virtual ~Mapper0();
  virtual uint8_t peek16(uint16_t address);

  Rom *rom;

private:
  // 32 KiB = 32768 = 0x8000
  uint8_t prg[0x8000] = {0};
};

class VM {
public:
  VM(Rom *rom);
  ~VM();

  // Methods
  uint8_t peek(Address::Absolute address);
  uint8_t peek8(uint8_t offset);
  uint8_t peek16(uint16_t address);

  void start();

private:
  /// Mapped from $0000-$07FF, with 3 mirrors from $0800-$1FF
  uint8_t ram[2048] = {0};

  /// Mapped from $2000-$2007
  uint8_t ppuRegisters[8] = {0};
  uint8_t apuAndIoRegisters[24] = {0};

  Mapper *mapper;

  // registers
  uint16_t PC = 0;
  uint8_t A = 0;
  uint8_t X = 0;
  uint8_t Y = 0;

  /// Status
  ///
  /// 7  bit  0
  /// ---- ----
  /// NV1B DIZC
  /// |||| ||||
  /// |||| |||+- Carry
  /// |||| ||+-- Zero
  /// |||| |+--- Interrupt Disable
  /// |||| +---- Decimal (NES no-op)
  /// |||+------ (CPU no-op; observable on the stack, though)
  /// ||+------- (no-op; always pushed as 1)
  /// |+-------- Overflow
  /// +--------- Negative
  uint8_t S = 0;

  /// Negative bitmask
  const uint8_t _N = 1 << 7;
  const uint8_t _NNot = ~_N;

  ///// Overflow bitmask
  //const uint8_t _V = 1 << 6;

  ///// Interrupt bitmask
  //const uint8_t _I = 1 << 2;

  /// Zero bitmask
  const uint8_t _Z = 1 << 1;
  const uint8_t _ZNot = ~_Z;

  /// Carry bitmask
  const uint8_t _C = 1 << 0;
  const uint8_t _CNot = ~_C;

  // Methods
  Instructions::Instruction decodeInstruction();
  void execute(Instructions::Instruction instruction);

  void inline _setN(uint8_t other);
  void inline _setZ(uint8_t other);
  void inline _setC(bool didCarry);
};

} // namespace VM
