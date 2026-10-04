#include <memory>
#ifdef NDEBUG
#include <cstdio>
#endif

#include "../include/debug.h"
#include "../include/rom.h"

int main(int argc, char **argv) {
  const char *romPath = nullptr;
  if (argc == 1) {
#ifdef NDEBUG
    fprintf(stderr, "Usage: bin.exe path-to-rom.nes\n");
    return 1;
#else
    romPath = "./rom.nes";
#endif
  } else {
    romPath = argv[1];
  }

  try {
    // No copy
    std::shared_ptr<Rom> p{new Rom(romPath)};
    Debugger debugger = {p};
    debugger.start();
  } catch (std::runtime_error e) {
    fprintf(stderr, "caught: %s\n", e.what());
    return 1;
  } catch (...) {
    fprintf(stderr, "Unknown error!\n");
    return 1;
  }
}
