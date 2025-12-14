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

Debugger::Debugger(VM::VM *_vm) : vm(_vm) { initscr(); }

Debugger::~Debugger() { endwin(); }

void Debugger::start() {
  addstr("Hi");

  vm->PC = {
      vm->peek16(0xFFFD), // high
      vm->peek16(0xFFFC), // low
  };

  Instructions::Instruction ins;
  while (1) {
    printw("%04X: ", vm->PC.to16());
    ins = vm->decodeInstruction();
    _renderInstruction(ins);
    vm->execute(ins);
    printRegisters();

    // TODO: this is sus, since getline expects this to be malloc allocated
    char inputLine[1024] = {0};
    // char *inputLinePtr = inputLine;
    size_t inputSize = 1024;

    addstr("? ");
    int nread = getnstr(inputLine, inputSize);
    // int nread = getline(&inputLinePtr, &inputSize, stdin);
    if (nread == EOF) {
      printw("\n\nExiting debugger.\n");
      exit(0);
    }
    if (nread == 0) {
      // step into
      continue;
    } else if (strncmp(inputLine, "setppu2", 5) == 0) {
      // TODO: is this right?
      // we're branching on if the zero flag is set, so don't branch
      vm->ppuRegisters[2] = 1 << 7;
      printw("Setting PPU[2] = #%02X\n", vm->ppuRegisters[2]);
      continue;
    } else {
      printw("strncmp() = %d\n", strncmp(inputLine, "A = ", 4));
      printw("nread = %d\t\"%02X %02X\"\n", nread, inputLine[0], inputLine[1]);
      throw std::runtime_error(
          std::format("Unrecognized debugger input\nnread = {}", nread));
    }
    printw("[%d | %ld] You typed: \"%s\"\n", nread, inputSize, inputLine);
  }
}

void Debugger::printRegisters() {
  printw("PC   A  X  Y  SP NV-BDIZC\n");
  printw("%04X %02X %02X %02X %02X %s\n\n", vm->PC.to16(), vm->A, vm->X, vm->Y,
         vm->SP, std::bitset<8>{vm->S}.to_string().data());
}

} // namespace Debug

} // namespace NESPP
