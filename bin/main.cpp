// Headless entrypoint

#include <cstdio>
#include <unistd.h>

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
  //printf("%X\n", rom->prgBlob[0]);
}

int main(int argc, char **argv) {
  if (argc == 1) {
    fprintf(stderr, "Usage: bin.exe path-to-rom.nes\n");
    return 1;
  }
  Rom *rom = new Rom();
  try {
    Rom::fromPath(&rom, argv[1]);
  } catch (const char *msg) {
    fprintf(stderr, "Caught %s!\n", msg);
    return 1;
  }

  printf("mapper = %d\n", rom->mapper);
  printf("Successfully loaded %s\n", argv[1]);

  debugPrg(rom);
  delete rom;
}
