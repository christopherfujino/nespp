#pragma once

#include <stdio.h>

#include "instructions.h"
#include "vm.h"

namespace Debug {

struct Debugger {
  VM::VM *vm;

  void start();
};

} // namespace Debug
