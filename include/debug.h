#pragma once

#include <stdio.h>

#include "vm.h"

namespace NESPP {

namespace Debug {

struct Debugger {
  VM::VM *vm;

  void start();
  void printRegisters();
};

} // namespace Debug

} // namespace NESPP
