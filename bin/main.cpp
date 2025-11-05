#include <cstdio>
#include <unistd.h>

#include "../include/rom.h"

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

  delete rom;
}
