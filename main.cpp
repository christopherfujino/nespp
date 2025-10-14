#include <cstdint>
#include <cstdio>
#include <cstring>
#include <unistd.h>

const size_t HEADER_SIZE = 16;

struct Rom {
  // Size of PRG ROM in 16 KB units
  int prgSize;

  // Size of CHR ROM in 8 KB units (value 0 means the board uses CHR RAM)
  int chrSize;

  // 6	Flags 6 – Mapper, mirroring, battery, trainer

  /// 0 vertical, 1 horizontal
  uint8_t nameTableArrangement;
  uint8_t batteryBackedPRGRam;

  /// 1: 512-byte trainer at $7000-$71FF (stored before PRG data)
  uint8_t trainer;

  /// 1: Alternative nametable layout
  uint8_t alternativeNametableLayout;

  uint8_t mapper;

  // 7	Flags 7 – Mapper, VS/Playchoice, NES 2.0
  // 8	Flags 8 – PRG-RAM size (rarely used extension)
  // 9	Flags 9 – TV system (rarely used extension)
  // 10	Flags 10 – TV system, PRG-RAM presence (unofficial, rarely used
  // extension) 11-15	Unused padding (should be filled with zero, but some
  // rippers put their name across bytes 7-15)
  uint8_t *prgBlob;
  uint8_t *chrBlob;
};

Rom romFromPath(const char *path) {
  Rom rom;
  uint8_t headerBytes[HEADER_SIZE];

  FILE *f = fopen(path, "rb");
  fread(&headerBytes, HEADER_SIZE, 1, f);

  if (headerBytes[0] != 'N' || headerBytes[1] != 'E' || headerBytes[2] != 'S' ||
      headerBytes[3] != 0x1a) {
    throw "Unknown ROM format!";
  }

  rom.prgSize = (int)headerBytes[4] * 1024 * 16;
  rom.chrSize = (int)headerBytes[5] * 1024 * 8;
  uint8_t flags6 = headerBytes[6];
  rom.nameTableArrangement = flags6 & 1;
  rom.batteryBackedPRGRam = flags6 & 1 << 1;
  rom.trainer = flags6 & 1 << 2;
  rom.alternativeNametableLayout = flags6 & 1 << 3;
  uint8_t lowerNibbleOfMapper = (flags6 & 0b11110000) >> 4;

  uint8_t flags7 = headerBytes[7];
  if (flags7 & 1) {
    throw "TODO: implement VS Unisystem";
  }
  if (flags7 & (1 << 1)) {
    throw "TODO: implement Playchoice-10";
  }
  if (flags7 & 0b1100) {
    throw "TODO: implement NES2.0";
  }
  uint8_t upperNibbleOfMapper = flags7 & 0b11110000;
  rom.mapper = upperNibbleOfMapper | lowerNibbleOfMapper;

  // TODO parse flags 8-10

  rom.prgBlob = new uint8_t[rom.prgSize];
  fread(rom.prgBlob, rom.prgSize, 1, f);

  rom.chrBlob = new uint8_t[rom.chrSize];
  fread(rom.chrBlob, rom.chrSize, 1, f);

  fclose(f);
  return rom;
}

int main() {
  try {
    Rom rom = romFromPath("./rom.nes");
  } catch (const char *msg) {
    fprintf(stderr, "Caught %s!\n", msg);
    _exit(1);
  }
}
