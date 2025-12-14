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

_Queue::_Queue(int _size) : size(_size) {}

void _Queue::enqueue(std::string element) {
  if (contents.size() == size) {
    contents.pop_front();
  }
  contents.push_back(element);
}

void _Queue::renderLines(int y, int x, int height, int width) {
  // TODO check if height < size
  int currentY = y;
  for (auto line : contents) {
    mvaddnstr(currentY, x, line.data(), width);
    currentY += 1;
  }
}

void _renderBox(int y, int x, int height, int width) {
  mvaddch(y, x, '+');
  hline('-', width - 2);
  mvaddch(y, x + width - 1, '+');
  mvaddch(y + height - 1, x, '+');
  hline('-', width - 2);
  mvaddch(y + height - 1, x + width - 1, '+');
  mvvline(y + 1, x, '|', height - 2);
  mvvline(y + 1, x + width - 1, '|', height - 2);
}

void _renderInstruction(Debugger *dbg) {
  constexpr int x = 30;
  constexpr int y = 0;
  constexpr int width = 25;
  // TODO take a height as an argument
  const int height = dbg->instructionQueue.size + 2;
  _renderBox(y, x, height, width);
  dbg->instructionQueue.renderLines(y + 1, x + 1, height - 2, width - 2);
}

void _renderRegisters(Debugger *dbg) {
  constexpr int x = 0;
  constexpr int y = 0;
  constexpr int width = 27;
  constexpr int height = 4;
  _renderBox(y, x, height, width);
  mvprintw(y + 1, x + 1, "PC   A  X  Y  SP NV-BDIZC");
  mvprintw(y + 2, x + 1, "%04X %02X %02X %02X %02X %s", dbg->PC.to16(), dbg->A,
           dbg->X, dbg->Y, dbg->SP, std::bitset<8>{dbg->S}.to_string().data());
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
    auto insLoc = PC;
    Instructions::Instruction ins = decodeInstruction();
    instructionQueue.enqueue(
        std::format("{:4X}: {}", insLoc.to16(), ins.toString().data()));
    clear();
    _renderInstruction(this);
    execute(ins);
    _renderRegisters(this);
    refresh();

    char inputLine[1024] = {0};
    size_t inputSize = 1024;

    mvaddstr(6, 0, "? ");
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
      debug(std::format("Setting PPU[2] = #{:2X}", ppuRegisters[2]));
      continue;
    } else {
      throw std::runtime_error(
          std::format("Unrecognized debugger input: \"{}\"", inputLine));
    }
  }
}

void Debugger::debug(std::string msg) {
  constexpr int y = 20;
  constexpr int x = 0;
  int height = debugQueue.size + 2;
  constexpr int width = 40;
  _renderBox(y, x, height, width);
  debugQueue.enqueue(msg);
  debugQueue.renderLines(y + 1, x + 1, height - 2, width - 2);
  // mvprintw(y, x, "%s", msg.data());
}

} // namespace Debug

} // namespace NESPP
