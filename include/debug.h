#pragma once

#include "rom.h"
#include "vm.h"
#include <list>

#include <string>

namespace NESPP {

namespace Debug {

class _Queue {
public:
  _Queue(int _size);

  void enqueue(std::string element);

  void renderLines(int y, int x, int height, int width);

  const int size;

private:
  std::list<std::string> contents;
};

class Debugger : public VM::VM {
public:
  Debugger(Rom *rom);
  ~Debugger();

  _Queue instructionQueue = {5};

  void start();

private:
  _Queue debugQueue = {5};
  virtual void debug(std::string) override;
};

} // namespace Debug

} // namespace NESPP
