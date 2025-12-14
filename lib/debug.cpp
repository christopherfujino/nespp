#include "../include/debug.h"
#include "../include/address.h"      // for Absolute
#include "../include/instructions.h" // for Instruction, OpCode
#include "../include/vm.h"           // for VM
#include <bitset>                    // std::bitset
#include <cstdlib>                   // for exit, size_t
#include <cstring>                   // for strncmp
#include <format>
#include <ncurses.h>
#include <stdexcept>
#include <stdint.h> // for uint8_t

namespace NESPP {

namespace Debug {

void _renderInstruction(Instructions::Instruction &ins) {
  printw("%s\n", ins.toString().data());
}

void _renderRegisters(Debugger *dbg) {
  printw("PC   A  X  Y  SP NV-BDIZC\n");
  printw("%04X %02X %02X %02X %02X %s\n\n", dbg->PC.to16(), dbg->A, dbg->X, dbg->Y, dbg->SP,
         std::bitset<8>{dbg->S}.to_string().data());
}

Debugger::Debugger(Rom *rom) : VM::VM(rom) { initscr(); }

Debugger::~Debugger() {
  printw("about to call endwin()\n");
  endwin();
  printf("called endwin()\n");
  fflush(stdout);
  VM::~VM();
}

void Debugger::start() {
  PC = {
      peek16(0xFFFD), // high
      peek16(0xFFFC), // low
  };

  while (1) {
    printw("%04X: ", PC.to16());
    Instructions::Instruction ins = decodeInstruction();
    _renderInstruction(ins);
    execute(ins);
    _renderRegisters(this);

    char inputLine[1024] = {0};
    size_t inputSize = 1024;

    addstr("? ");
    int result = wgetnstr(stdscr, inputLine, inputSize);
    // int nread = getline(&inputLinePtr, &inputSize, stdin);
    if (result == ERR) {
      throw std::runtime_error("wgetnstr() hit an error...");
    }
    if (strncmp(inputLine, "", 1) == 0) {
      // step into
      continue;
    } else if (strncmp(inputLine, "setppu2", 7) == 0) {
      // TODO: is this right?
      // we're branching on if the zero flag is set, so don't branch
      ppuRegisters[2] = 1 << 7;
      printw("Setting PPU[2] = #%02X\n", ppuRegisters[2]);
      continue;
    } else {
      throw std::runtime_error(
          std::format("Unrecognized debugger input: \"{}\"", inputLine));
    }
  }
}

void Debugger::debug(std::string msg) {
  printw("%s", msg.data());
}

} // namespace Debug

} // namespace NESPP
