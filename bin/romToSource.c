#include "common.h" // panic, Result
#include "raylib.h"
#include "rom.h"   // Rom
#include <stdio.h> // fprintf(), stderr

int main(int argc, char **argv) {
  constexpr int TILE_WIDTH = 8; // TODO?
  if (argc == 1) {
    fprintf(stderr, "Usage: bin.exe path-to-rom.nes\n");
    return 1;
  }
  Rom rom;
  Result res = romNew(&rom, argv[1]);
  if (res.tag != resultOk_t) {
    panic("romNew() failed: %s\n", res.err);
  }
  const size_t tileCount = rom.chrSize / TILE_SIZE;
  Image img = GenImageColor(tileCount * TILE_WIDTH, 8,
                            (Color){.r = 0, .g = 0, .b = 0, .a = 0xFF});
  for (size_t tile = 0; tile < tileCount; tile++) {
    // for (size_t tile = 0; tile < 10; tile++) {
    const auto tileOffset = tile * TILE_SIZE;

    for (int byteOffset = 0; byteOffset < 8; byteOffset++) {
      uint8_t plane0Byte = rom.chrBlob[tileOffset + byteOffset];
      uint8_t plane1Byte = rom.chrBlob[tileOffset + byteOffset + 8];

      for (int bitOffset = 0; bitOffset < 8; bitOffset++) {
        uint8_t lowerBit = plane0Byte >> (7 - bitOffset) & 0x1;
        uint8_t upperBit = plane1Byte >> (7 - bitOffset) & 0x1;
        // range [0, 3]
        uint8_t bit = lowerBit | (upperBit << 1);

        int x = tile * TILE_WIDTH + bitOffset;
        int y = byteOffset;
        // This divides nicely
        const unsigned char colorByte = (255 / 3) * bit;
        ImageDrawPixel(&img, x, y,
                       (Color){
                           .r = colorByte,
                           .g = colorByte,
                           .b = colorByte,
                           .a = 0xFF,
                       });
      }
    }
  }
  if (!ExportImage(img, "flooboo.png")) {
    panic("ExportImage() failed!\n");
  }
}
