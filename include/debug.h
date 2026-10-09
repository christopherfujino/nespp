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

struct Debugger {
  VM super;

  _Queue instructionQueue = {5};
  _Queue debugQueue = {30};
};

Debugger debuggerNew(std::shared_ptr<Rom> rom);
void start();
void render();
void debug(std::string);
