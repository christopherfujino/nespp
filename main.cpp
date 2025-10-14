#include <cstdio>
#include <unistd.h>

#include "rom.hpp"

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
  rom.renderCHR();
}
