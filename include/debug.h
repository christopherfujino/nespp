#ifndef __MONOREPO_SRC_NESPP_INCLUDE_DEBUG_H
#define __MONOREPO_SRC_NESPP_INCLUDE_DEBUG_H

#include "common.h"
#include "rom.h"
#include "vm.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef const char *_QueueT;
typedef struct {
  // TODO: when in C, make this `const`
  size_t size;

  _QueueT *contents;
  _QueueT *start;
  _QueueT *next;
} _Queue;

typedef struct Debugger {
  VM super;

  _Queue instructionQueue;
  _Queue debugQueue;
} Debugger;

Debugger debuggerNew(Rom *rom);
Result debuggerStart(Debugger *debugger);
void debuggerRender(Debugger *debugger);
void debuggerDispose(Debugger *debugger);
void debuggerDumpQueue(Debugger *debugger);

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // __MONOREPO_SRC_NESPP_INCLUDE_DEBUG_H
