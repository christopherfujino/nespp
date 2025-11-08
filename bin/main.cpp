// Headless entrypoint

#include <cstdio>
#include <unistd.h>

// #include "../include/instructions.h"
#include "../include/rom.h"
#include "../include/vm.h"

void debug(uint8_t *blob, size_t size) {
  printf("size = %ld bytes\n", size);
  for (int i = 0; i < size; i += 16) {
    printf("%04X:", i);
    for (int j = i; j < i + 16; j += 1) {
      printf(" %02X", blob[j]);
    }
    putchar('\n');
  }
}

int run(char *path) {
  Rom *rom = new Rom();

  Rom::fromPath(&rom, path);
  VM::VM vm = VM::VM(rom);

  printf("mapper = %d\n", rom->mapper);
  printf("Successfully loaded %s\n", path);


  // debug(rom->chrBlob, rom->chrSize);
  //  debug(rom->prgBlob, rom->prgSize);
  //  auto foo = Instructions::deserialize(rom->prgBlob, rom->prgSize);
  //  delete rom;
  vm.start();
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
    fprintf(stderr, "[Error] %s!\n", msg);
    return 1;
  } catch (char const *msg) {
    fprintf(stderr, "[Error] %s!\n", msg);
    return 1;
  }
}
