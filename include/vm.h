#ifndef __MONOREPO_SRC_NESPP_INCLUDE_VM_H
#define __MONOREPO_SRC_NESPP_INCLUDE_VM_H

#include <stdint.h>

#include "instructions.h"
struct Rom; // #include "rom.h"
#include "word.h"

struct Mapper0 {
  Rom *rom;

  // 32 KiB = 32768 = 0x8000
  uint8_t prg[0x8000] = {0};
};

Mapper0 mapper0New(Rom *rom);
uint8_t mapper0Peek16(Mapper0 *mapper, uint16_t address);
void mapper0Poke16(Mapper0 *mapper, uint16_t address, uint8_t value);

// TODO: inline when we're in C world
enum _MapperTag {
  mapperTag0,
};

typedef struct Mapper {
  _MapperTag tag;
  union {
    Mapper0 mapper0;
  };
} Mapper;

uint8_t mapperPeek16(Mapper *mapper, uint16_t address);
void mapperPoke16(Mapper *mapper, uint16_t address, uint8_t value);

typedef struct VM {
  // registers
  Word PC;
  uint8_t A;
  uint8_t X;
  uint8_t Y;

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
  uint8_t S;

  // Memory

  /// Mapped from $0000-$07FF, with 3 mirrors from $0800-$1FF
  uint8_t ram[2048];

  /// Mapped from $2000-$2007
  uint8_t ppuRegisters[8];
  uint8_t apuAndIoRegisters[24];

  Mapper mapper;

  Rom *rom;

  void (*debug)(VM *, std::string);
} VM;

VM vmNew(Rom *_rom, void (*debug)(VM *, std::string));

void vmPoke(VM *vm, Word address, uint8_t value);
void vmPoke16(VM *vm, uint16_t address, uint8_t value);

Instruction vmDecodeInstruction(VM *vm);
void vmExecute(VM *vm, Instruction instruction);

void vmStart(VM *vm);

uint8_t vmPeek(VM *vm, Word address);
uint8_t vmPeek8(VM *vm, uint8_t offset);
uint8_t vmPeek16(VM *vm, uint16_t address);

#endif // __MONOREPO_SRC_NESPP_INCLUDE_VM_H
