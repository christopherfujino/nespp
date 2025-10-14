#include <cstdio>
#include <unistd.h>

#include "rom.hpp"

int main() {
  Rom rom;
  try {
    rom = Rom::fromPath("./rom.nes");
  } catch (const char *msg) {
    fprintf(stderr, "Caught %s!\n", msg);
    _exit(1);
  }
  rom.renderCHR();
}
