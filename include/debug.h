#pragma once

#include "rom.h"
#include "vm.h"

#include <string>

namespace NESPP {

namespace Debug {

class Debugger : public VM::VM {
public:
  Debugger(Rom *rom);
  ~Debugger();

  void start();
private:
  virtual void debug(std::string) override;
};

} // namespace Debug

} // namespace NESPP
