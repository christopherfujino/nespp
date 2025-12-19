#ifdef NDEBUG
#include <cstdio>
#endif

#include "../include/debug.h"
#include "../include/rom.h"

using namespace NESPP;

int main(int argc, char **argv) {
  const char *romPath = nullptr;
  if (argc == 1) {
#ifdef NDEBUG
    fprintf(stderr, "Usage: bin.exe path-to-rom.nes\n");
    return 1;
#else
    romPath = "./build/rom.nes";
#endif
  } else {
    romPath = argv[1];
  }

  Rom rom = Rom();

  Rom::fromPath(&rom, romPath);

  Debugger debugger = {&rom};
  debugger.start();
}
