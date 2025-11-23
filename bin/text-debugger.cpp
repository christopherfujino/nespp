#include <cstdio>

#include "../include/rom.h"
#include "../include/vm.h"

int main(int argc, char **argv) {
  if (argc == 1) {
    fprintf(stderr, "Usage: bin.exe path-to-rom.nes\n");
    return 1;
  }

  Rom *rom = new Rom();

  Rom::fromPath(&rom, argv[1]);
  VM::VM vm = VM::VM(rom);
}
