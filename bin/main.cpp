// Headless entrypoint

#include <cstdio>
#include <unistd.h>

#include "../include/instructions.h"
#include "../include/rom.h"

void debugPrg(Rom *rom) {
  printf("prgSize = %d\n", rom->prgSize);
  for (int i = 0; i < rom->prgSize; i += 16) {
    printf("%04x:", i);
    for (int j = i; j < i + 16; j += 1) {
      printf(" %02x", rom->prgBlob[j]);
    }
    printf("\n");
  }
  // printf("%X\n", rom->prgBlob[0]);
}

int run(char *path) {
  Rom *rom = new Rom();

  Rom::fromPath(&rom, path);

  printf("mapper = %d\n", rom->mapper);
  printf("Successfully loaded %s\n", path);

  // debugPrg(rom);
  auto foo = Instructions::deserialize(rom->prgBlob, rom->prgSize);
  delete rom;
  return 0;
}

int main(int argc, char **argv) {
  if (argc == 1) {
    fprintf(stderr, "Usage: bin.exe path-to-rom.nes\n");
    return 1;
  }

  try {
    return run(argv[1]);
  } catch (char *msg) {
    fprintf(stderr, "Caught %s!\n", msg);
    return 1;
  }
}
