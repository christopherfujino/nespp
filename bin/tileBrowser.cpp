#include <cstdio>

#include "../THIRD_PARTY/raylib-5.5/out/raylib/include/raylib.h"
#include "../include/rom.hpp"

void render(Rom *rom);
void renderTile(Rom *rom, int i);

int main(int argc, char **argv) {
  if (argc == 1) {
    fprintf(stderr, "Usage: bin.exe path-to-rom.nes\n");
    return 1;
  }
  Rom rom;
  try {
    rom = Rom::fromPath(argv[1]);
  } catch (const char *msg) {
    fprintf(stderr, "Caught %s!\n", msg);
    return 1;
  }
  render(&rom);
}

void render(Rom *rom) {
  // Decode n tiles
  const int n = rom->chrSize / Rom::TILE_SIZE;

  InitWindow(Rom::SCREEN_WIDTH, Rom::SCREEN_HEIGHT, "NES");
  SetTargetFPS(10);

  const int MESSAGE_MAX = 100;
  char msg[MESSAGE_MAX] = {0};

  int tileNumber = 0;
  while (!WindowShouldClose()) {
    if (IsKeyDown(KEY_L)) {
      tileNumber += 1;
      if (tileNumber >= n) {
        tileNumber = n - 1;
      }
    } else if (IsKeyDown(KEY_H)) {
      tileNumber -= 1;
      if (tileNumber < 0) {
        tileNumber = 0;
      }
    }
    BeginDrawing();

    ClearBackground(BLACK);

    renderTile(rom, tileNumber);

    snprintf(msg, MESSAGE_MAX, "%d/%d", tileNumber, n);
    DrawText(msg, 5 * Rom::PIXEL_SCALE, 20 * Rom::PIXEL_SCALE, 96, WHITE);

    EndDrawing();
  }
}

void renderTile(Rom *rom, int i) {
  int x = 5;
  int y = 5;

  std::size_t tileOffset = i * (Rom::TILE_SIZE);
  for (int byteOffset = 0; byteOffset < 8; byteOffset++) {
    uint8_t plane0Byte = rom->chrBlob[tileOffset + byteOffset];
    uint8_t plane1Byte = rom->chrBlob[tileOffset + byteOffset + 8];

    for (int bitOffset = 0; bitOffset < 8; bitOffset++) {
      uint8_t lowerBit = plane0Byte >> (7 - bitOffset) & 0x1;
      uint8_t upperBit = plane1Byte >> (7 - bitOffset) & 0x1;
      uint8_t bit = lowerBit | (upperBit << 1);

      // int x = ((i % TILES_PER_ROW) * 8 + bitOffset);
      // int y = ((i * 8 / TILES_PER_ROW) + byteOffset);
      DrawRectangleV(
          // position
          (Vector2){.x = (float)((x + bitOffset) * Rom::PIXEL_SCALE),
                    .y = (float)((y + byteOffset) * Rom::PIXEL_SCALE)},
          // size
          (Vector2){.x = Rom::PIXEL_SCALE, .y = Rom::PIXEL_SCALE},
          (Color){
              .r = 0xFF,
              .g = 0xFF,
              .b = 0xFF,
              .a = (unsigned char)(0xFF / 3 * bit),
          });
    }
  }
}
