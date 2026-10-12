#ifndef __MONOREPO_SRC_NESPP_INCLUDE_DEBUG_H
#define __MONOREPO_SRC_NESPP_INCLUDE_DEBUG_H

struct Rom; // #include "rom.h"
#include "vm.h"

#include <string>

typedef std::string _QueueT;
struct _Queue {
  // TODO: when in C, make this `const`
  size_t size;

  _QueueT *contents;
  _QueueT *start;
  _QueueT *next;
};

struct Debugger {
  VM super;

  _Queue instructionQueue;
  _Queue debugQueue;
};

Debugger debuggerNew(Rom *rom);
void debuggerStart(Debugger *debugger);
void debuggerStart(Debugger *debugger);
void debuggerRender(Debugger *debugger);
void debuggerDispose(Debugger *debugger);

#endif // __MONOREPO_SRC_NESPP_INCLUDE_DEBUG_H
