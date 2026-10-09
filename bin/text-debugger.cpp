#include <stdexcept>
#ifdef NDEBUG
#include <cstdio>
#endif

#include "../include/debug.h"
#include "../include/rom.h"
#include "../include/vm.h"

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

  int exitCode = 0;
  Debugger debugger = {};
  constexpr size_t msgBufferLen = 512;
  char msgBuffer[msgBufferLen] = {0};
  try {
    auto rom = Rom(romPath);
    debugger = debuggerNew(&rom);
    debuggerStart(&debugger);
  } catch (std::runtime_error e) {
    snprintf(msgBuffer, msgBufferLen, "caught: %s\n", e.what());
    exitCode = 1;
  } catch (...) {
    snprintf(msgBuffer, msgBufferLen, "Unknown error!\n");
    exitCode = 1;
  }

  debuggerDispose(&debugger);
  if (msgBuffer[0]) {
    fprintf(stderr, "%s", msgBuffer);
  } else {
    fprintf(stderr, "empty msgBuffer\n");
  }
  return exitCode;
}
