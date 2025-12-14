#pragma once

#include "vm.h"

namespace NESPP {

namespace Debug {

class Debugger {
public:
  Debugger(VM::VM *);
  ~Debugger();

  void start();
private:
  VM::VM *vm;

  void printRegisters();
};

} // namespace Debug

} // namespace NESPP
