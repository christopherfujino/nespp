#pragma once

#include <stdio.h>

#include "instructions.h"
#include "rom.h"

namespace Debug {

void instruction(Instructions::Instruction instruction);

class Debugger {
public:
  Rom *rom;
  void input();
};

} // namespace Debug
