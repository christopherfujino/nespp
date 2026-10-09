#pragma once

struct Rom; // #include "rom.h"
#include "vm.h"
#include <list>

#include <string>

struct _Queue {
  // TODO: when in C, make this `const`
  unsigned int size;

  std::list<std::string> contents;
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

void debug(std::string);
