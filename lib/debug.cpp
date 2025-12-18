#include "../include/debug.h"
#include "../include/word.h"      // for Word
#include "../include/instructions.h" // for Instruction, OpCode
#include "../include/vm.h"           // for VM
#include <bitset>                    // std::bitset
#include <cstdlib>                   // for exit, size_t
#include <cstring>                   // for strncmp
#include <format>
#include <locale.h>
#include <ncurses.h>
#include <stdexcept>
#include <stdint.h> // for uint8_t

namespace NESPP {

_Queue::_Queue(int _size) : size(_size) {}

void _Queue::enqueue(std::string element) {
  if (contents.size() == size) {
    contents.pop_front();
  }
  contents.push_back(element);
}

void _Queue::renderLines(int y, int x, int _, int width) {
  // TODO check if height < size
  int currentY = y;
  for (auto line : contents) {
    mvaddnstr(currentY, x, line.data(), width);
    currentY += 1;
  }
}

void _renderBox(int y, int x, int height, int width) {
  mvaddch(y, x, A_ALTCHARSET | ACS_ULCORNER);
  hline(A_ALTCHARSET | ACS_HLINE, width - 2);
  mvaddch(y, x + width - 1, A_ALTCHARSET | ACS_URCORNER);
  mvaddch(y + height - 1, x, A_ALTCHARSET | ACS_LLCORNER);
  hline(A_ALTCHARSET | ACS_HLINE, width - 2);
  mvaddch(y + height - 1, x + width - 1, A_ALTCHARSET | ACS_LRCORNER);
  mvvline(y + 1, x, A_ALTCHARSET | ACS_VLINE, height - 2);
  mvvline(y + 1, x + width - 1, A_ALTCHARSET | ACS_VLINE, height - 2);
}

void _renderDebug(Debugger *dbg) {
  constexpr int y = 13;
  constexpr int x = 0;
  int height = dbg->debugQueue.size + 2;
  constexpr int width = 50;
  _renderBox(y, x, height, width);

  dbg->debugQueue.renderLines(y + 1, x + 1, height - 2, width - 2);
}

void _renderInstruction(Debugger *dbg) {
  constexpr int x = 0;
  constexpr int y = 4;
  constexpr int width = 30;
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

void _renderStack(Debugger *dbg) {
  constexpr int x = 30;
  constexpr int y = 0;
  constexpr int width = 10;
  constexpr int height = 13;
  for (int i = 0; i < height; i++) {
    uint16_t ptr = dbg->SP + i + 0x0100;
    if (ptr > 0x01FF) {
      break;
    }
    auto val = dbg->peek16(ptr);
    mvprintw(y + i + 1, x + 1, "%04X: %02X", ptr, val);
  }
  _renderBox(y, x, height, width);
}

Debugger::Debugger(Rom *rom) : VM::VM(rom) {
  setlocale(LC_ALL, "en_US.UTF-8");
  initscr();
  // noecho();
}

Debugger::~Debugger() {
  printw("about to call endwin()\n");
  endwin();
  printf("called endwin()\n");
  fflush(stdout);
  VM::~VM();
}

void Debugger::render() {
  clear();
  _renderInstruction(this);
  _renderRegisters(this);
  _renderStack(this);
  _renderDebug(this);

  // prompt
  mvaddstr(instructionQueue.size + 6, 0, "> ");
  refresh();
}

void Debugger::start() {
  PC = {
      peek16(0xFFFD), // high
      peek16(0xFFFC), // low
  };

  while (1) {
    auto insLoc = PC;
    Instruction ins = decodeInstruction();
    instructionQueue.enqueue(
        std::format("{:4X}: {}", insLoc.to16(), ins.toString().data()));
    execute(ins);
    render();

    constexpr size_t inputSize = 1024;
    char inputLine[inputSize] = {0};
    int result = getnstr(inputLine, inputSize);
    if (result == ERR) {
      throw std::runtime_error("getnstr returned ERR");
    }
    if (strncmp(inputLine, "", 1) == 0) {
      // step into
      continue;
    } else if (strncmp(inputLine, "setppu2", 7) == 0) {
      // TODO: is this right?
      // we're branching on if the zero flag is set, so don't branch
      ppuRegisters[2] = 1 << 7;
      debug(std::format("Setting PPU[2] = #{:02X}", ppuRegisters[2]));
      continue;
    } else {
      throw std::runtime_error(
          std::format("Unrecognized debugger input: \"{}\" ({})", inputLine,
                      strlen(inputLine)));
    }
  }
}

void Debugger::debug(std::string msg) { debugQueue.enqueue(msg); }

} // namespace NESPP
