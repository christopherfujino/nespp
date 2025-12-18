#pragma once

#include "rom.h"
#include "vm.h"
#include <list>

#include <string>

namespace NESPP {

class _Queue {
public:
  _Queue(int _size);

  void enqueue(std::string element);

  void renderLines(int y, int x, int height, int width);

  const unsigned int size;

private:
  std::list<std::string> contents;
};

class Debugger : public VM::VM {
public:
  Debugger(Rom *rom);
  ~Debugger();

  _Queue instructionQueue = {5};
  _Queue debugQueue = {8};

  void start();

private:
  void render();
  virtual void debug(std::string) override;
};

} // namespace NESPP
