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

#include "rom.hpp"
#include "raylib.h"
#include <cstdio>
#include <cstring>

Rom Rom::fromPath(const char *path) {
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
  printf("mapper = %d\n", rom.mapper);

  // TODO parse flags 8-10

  rom.prgBlob = new uint8_t[rom.prgSize];
  fread(rom.prgBlob, rom.prgSize, 1, f);

  rom.chrBlob = new uint8_t[rom.chrSize];
  fread(rom.chrBlob, rom.chrSize, 1, f);

  fclose(f);
  return rom;
}

inline std::size_t Rom::prgStart() { return HEADER_SIZE; }

inline std::size_t Rom::chrStart() { return this->prgStart() + this->prgSize; }

void Rom::renderCHR() {
  //   RGB(0,0,0)
  // const char black[] = "\033[30m\u2588\033[0m";
  const char black[] = " ";
  // RGB(85,85,85)
  const char brightBlack[] = "\033[90m\u2588\033[0m";
  // RGB(170,170,170)
  const char white[] = "\033[37m\u2588\033[0m";
  // RGB(255,255,255)
  const char brightWhite[] = "\033[97m\u2588\033[0m";

  const char *colorMatrix[] = {
      black,
      brightBlack,
      white,
      brightWhite,
  };

  // Decode n tiles
  const int n = this->chrSize / TILE_SIZE;

  const auto SCREEN_WIDTH = 1280;
  const auto PIXEL_SCALE = 4;
  InitWindow(SCREEN_WIDTH, 240, "NES");
  while (!WindowShouldClose()) {
    BeginDrawing();

    for (int tileNumber = 0; tileNumber < (SCREEN_WIDTH / (8 * PIXEL_SCALE)); tileNumber++) {
      // for (int tileNumber = 0; tileNumber < n; tileNumber++) {
      std::size_t tileOffset = tileNumber * (TILE_SIZE);
      for (int byteOffset = 0; byteOffset < 8; byteOffset++) {
        uint8_t plane0Byte = this->chrBlob[tileOffset + byteOffset];
        uint8_t plane1Byte = this->chrBlob[tileOffset + byteOffset + 8];

        for (int bitOffset = 0; bitOffset < 8; bitOffset++) {
          uint8_t lowerBit = plane0Byte >> (7 - bitOffset) & 0x1;
          uint8_t upperBit = plane1Byte >> (7 - bitOffset) & 0x1;
          uint8_t bit = lowerBit | (upperBit << 1);
          DrawRectangleV(
              // position
              (Vector2){.x = (float)((tileNumber * 8 + bitOffset) * PIXEL_SCALE),
                        .y = (float)(byteOffset * PIXEL_SCALE)},
              // size
              (Vector2){.x = PIXEL_SCALE, .y = PIXEL_SCALE},
              (Color){
                  .r = 0xFF,
                  .g = 0xFF,
                  .b = 0xFF,
                  .a = (unsigned char)(0xFF / 3 * bit),
              });
          // DrawPixel(tileNumber * 8 + bitOffset, byteOffset,
          //           (Color){
          //               .r = 0xFF,
          //               .g = 0xFF,
          //               .b = 0xFF,
          //               .a = (unsigned char)(0xFF / 3 * bit),
          //           });
        }
      }
    }

    EndDrawing();
  }
  return; // TODO delete

  for (int tileNumber = 0; tileNumber < n; tileNumber++) {
  std:
    size_t tileOffset = tileNumber * (TILE_SIZE);
    for (int byteOffset = 0; byteOffset < 8; byteOffset++) {
      uint8_t plane0Byte = this->chrBlob[tileOffset + byteOffset];
      uint8_t plane1Byte = this->chrBlob[tileOffset + byteOffset + 8];

      // Since we're printing from least sig to most sig
      for (int bitOffset = 7; bitOffset >= 0; bitOffset--) {
        uint8_t lowerBit = (plane0Byte >> bitOffset) & 0x1;
        uint8_t upperBit = (plane1Byte >> bitOffset) & 0x1;
        uint8_t bit = lowerBit | (upperBit << 1);
        printf("%s", colorMatrix[bit]);
      }
      printf("\n");
    }
  }
}
