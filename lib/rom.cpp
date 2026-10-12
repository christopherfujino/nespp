// Bytes	Description
// 0-3	Constant $4E $45 $53 $1A (ASCII "NES" followed by MS-DOS end-of-file)
// 4	Size of PRG ROM in 16 KB units
// 5	Size of CHR ROM in 8 KB units (value 0 means the board uses CHR RAM)
// 6	Flags 6 – Mapper, mirroring, battery, trainer
// 7	Flags 7 – Mapper, VS/Playchoice, NES 2.0
// 8	Flags 8 – PRG-RAM size (rarely used extension)
// 9	Flags 9 – TV system (rarely used extension)
// 10	Flags 10 – TV system, PRG-RAM presence (unofficial, rarely used
// extension) 11-15	Unused padding (should be filled with zero, but some
// rippers put their name across bytes 7-15)

#include <cstdint>
#include <cstdio>
#include <cstdlib> // malloc()

#include "../include/rom.h"
#include "common.h"

Result romNew(Rom *rom, const char *path) {
  uint8_t headerBytes[HEADER_SIZE];

  FILE *f = fopen(path, "rb");
  if (f == nullptr) {
    constexpr size_t buflen = 128;
    char *buf = (char *)malloc(buflen);
    snprintf(buf, buflen, "Failed to open a ROM at %s", path);
    return resultErr(buf);
  }
  fread(&headerBytes, HEADER_SIZE, 1, f);

  if (headerBytes[0] != 'N' || headerBytes[1] != 'E' || headerBytes[2] != 'S' ||
      headerBytes[3] != 0x1a) {
    return resultErr("Unknown ROM format!");
  }

  rom->prgSize = (int)headerBytes[4] * 1024 * 16;
  rom->chrSize = (int)headerBytes[5] * 1024 * 8;
  uint8_t flags6 = headerBytes[6];
  rom->nameTableArrangement = flags6 & 1;
  rom->batteryBackedPRGRam = flags6 & 1 << 1;
  rom->trainer = flags6 & 1 << 2;
  rom->alternativeNametableLayout = flags6 & 1 << 3;
  uint8_t lowerNibbleOfMapper = (flags6 & 0b11110000) >> 4;

  uint8_t flags7 = headerBytes[7];
  if (flags7 & 1) {
    return resultErr("TODO: implement VS Unisystem");
  }
  if (flags7 & (1 << 1)) {
    return resultErr("TODO: implement Playchoice-10");
  }
  if (flags7 & 0b1100) {
    return resultErr("TODO: implement NES2.0");
  }
  uint8_t upperNibbleOfMapper = flags7 & 0b11110000;
  rom->mapper = upperNibbleOfMapper | lowerNibbleOfMapper;

  // TODO parse flags 8-10

  rom->prgBlob = (uint8_t *)malloc(sizeof(uint8_t) * rom->prgSize);
  fread(rom->prgBlob, rom->prgSize, 1, f);
  // printf("read %d bytes of PRG ROM.\n", rom->prgSize);

  rom->chrBlob = (uint8_t *)malloc(sizeof(uint8_t) * rom->chrSize);
  fread(rom->chrBlob, rom->chrSize, 1, f);
  // printf("read %d bytes of CHR ROM.\n", rom->chrSize);

  fclose(f);

  return resultOk();
}

void romDispose(Rom *rom) {
  free(rom->prgBlob);
  free(rom->chrBlob);
}
