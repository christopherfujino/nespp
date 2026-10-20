#include <linux/prctl.h> // PR_SET_PTRACER
#include <stdexcept>     // std::runtime_error
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/types.h>

#include "debug.h"
#include "rom.h"

#define _USAGE()                                                               \
  do {                                                                         \
    printf("Usage: TODO\n");                                                   \
    for (int i = 0; i < argc; i++) {                                           \
      printf("\t%d: %s\n", i, argv[i]);                                        \
    }                                                                          \
    abort();                                                                   \
  } while (0)

typedef struct {
  const char *romPath;
  pid_t pid;
} Config;

static inline Config parse(int argc, char **argv) {
  auto config = Config{
      .romPath = "./rom.nes",
      .pid = 0,
  };

  if (argc == 1) {
    return config;
  }

  for (int i = 1; i < argc; i++) {
    if (strcmp("--rom", argv[i]) == 0) {
      printf("[DEBUG] match at %d\n", i);
      i += 1;
      config.romPath = argv[i];
      printf("[DEBUG] ROM at %d: %s\n", i, config.romPath);
      continue;
    }
    if (strcmp("--debugger-pid", argv[i]) == 0) {
      i += 1;
      int pid = atoi(argv[i]);
      if (pid == 0) {
        _USAGE();
      }
      config.pid = pid;
      continue;
    }
    _USAGE();
  }

  return config;
}

int main(int argc, char **argv) {
  Config config = parse(argc, argv);
  if (config.pid != 0) {
    int res = prctl(PR_SET_PTRACER, config.pid, 0, 0, 0);
    if (res < 0) {
      perror("prctl() failed");
      return 1;
    }
  }

  int exitCode = 0;
  Debugger debugger = {};
  constexpr size_t msgBufferLen = 512;
  char msgBuffer[msgBufferLen] = {0};
  try {
    Rom rom;
    auto result = romNew(&rom, config.romPath);
    switch (result.tag) {
    case resultOk_t:
      debugger = debuggerNew(&rom);
      debuggerStart(&debugger);
      break;
    case resultErr_t:
      strncpy(msgBuffer, result.err.s, msgBufferLen);

      break;
    }
  } catch (std::runtime_error err) {
    snprintf(msgBuffer, msgBufferLen, "ERR(%s): migrate this to `Result`!\n",
             err.what());
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
