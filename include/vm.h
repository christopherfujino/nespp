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
  virtual void poke16(uint16_t address, uint8_t value) = 0;
};

class Mapper0 : public Mapper {
public:
  Mapper0(Rom *rom);
  virtual ~Mapper0();
  virtual uint8_t peek16(uint16_t address);
  virtual void poke16(uint16_t address, uint8_t value);

  Rom *rom;

private:
  // 32 KiB = 32768 = 0x8000
  uint8_t prg[0x8000] = {0};
};

class VM {
public:
  VM(Rom *rom);
  ~VM();

  // registers
  uint16_t PC = 0;
  uint8_t A = 0;
  uint8_t X = 0;
  uint8_t Y = 0;

  /// Stack pointer
  ///
  /// Grows down from $FF to $00. These are offsets from CPU RAM map $0100.
  uint8_t SP = 0xFF;

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

  // Methods
  void start();

  uint8_t peek(Address::Absolute address);
  uint8_t peek8(uint8_t offset);
  uint8_t peek16(uint16_t address);

  void poke(Address::Absolute address, uint8_t value);
  void poke16(uint16_t address, uint8_t value);

  Instructions::Instruction decodeInstruction();
  void execute(Instructions::Instruction instruction);
private:
  Rom *rom = nullptr;

  /// Mapped from $0000-$07FF, with 3 mirrors from $0800-$1FF
  uint8_t ram[2048] = {0};

  /// Mapped from $2000-$2007
  uint8_t ppuRegisters[8] = {0};
  uint8_t apuAndIoRegisters[24] = {0};

  Mapper *mapper;

  /// Negative bitmask
  const uint8_t _N = 1 << 7;
  const uint8_t _NNot = ~_N;

  ///// Overflow bitmask
  // const uint8_t _V = 1 << 6;

  const uint8_t _D = 1 << 3;
  const uint8_t _DNot = ~_D;

  // Interrupt bitmask
  const uint8_t _I = 1 << 2;

  /// Zero bitmask
  const uint8_t _Z = 1 << 1;
  const uint8_t _ZNot = ~_Z;

  /// Carry bitmask
  const uint8_t _C = 1 << 0;
  const uint8_t _CNot = ~_C;

  // Methods

  void inline _setN(uint8_t other);
  void inline _setZ(uint8_t other);
  void inline _setC(bool didCarry);
};

} // namespace VM
