#pragma once

#include "vm.h"

namespace NESPP {

namespace Debug {

class Debugger {
public:
  Debugger(VM::VM *);
  ~Debugger();

  void start();
  void printRegisters();

private:
  VM::VM *vm;
};

} // namespace Debug

} // namespace NESPP
