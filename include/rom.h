#include <cstdint>
#include <stddef.h>

struct Rom {
  static Rom fromPath(const char *path);

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

  static const size_t HEADER_SIZE = 16;

  // 8 bytes for plane 0, 8 bytes for plane 1
  static const size_t TILE_SIZE = 16;

  static const int SCREEN_WIDTH = 1920;
  static const int SCREEN_HEIGHT = 1080;
  static const int PIXEL_SCALE = 8;
  static const int TILE_WIDTH = 8 * PIXEL_SCALE;
  static const int TILES_PER_ROW = SCREEN_WIDTH / TILE_WIDTH;

private:
  inline size_t prgStart();
  inline size_t chrStart();
};
