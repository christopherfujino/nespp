#pragma once

struct Rom; // #include "rom.h"
#include "vm.h"
#include <list>

#include <string>

struct _Queue {
  _Queue(int _size);

  void enqueue(std::string element);

  void renderLines(int y, int x, int height, int width);

  const unsigned int size;

  std::list<std::string> contents;
};

class Debugger : public VM::VM {
public:
  Debugger(std::shared_ptr<Rom> rom);
  ~Debugger();

  _Queue instructionQueue = {5};
  _Queue debugQueue = {30};

  void start();

private:
  void render();
  virtual void debug(std::string) override;
};
