#pragma once

#include <cstdint>

#include "address.h"
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
  uint16_t pc = 0;
};

} // namespace VM
