#include <bitset>                    // std::bitset
#include <cstdio>
#include <cstring> // for strncmp
#include <format>
#include <locale.h>
#include <ncurses.h>
#include <stdexcept>
#include <stdint.h> // for uint8_t
#include <stdio.h>  // sprintf()

#include "common.h"
#include "debug.h"
#include "instructions.h" // for Instruction, OpCode
#include "vm.h"           // for VM
#include "word.h"         // for Word

static inline size_t _queueLength(_Queue *queue) {
  if (queue->start == queue->next) {
    return 0;
  }

  if (queue->next > queue->start) {
    return queue->next - queue->start;
  }

  auto pastEnd = queue->contents + queue->size;
  return pastEnd - queue->start + queue->next - queue->contents;
}

static inline _QueueT *_maybeWrap(size_t len, _QueueT *start, _QueueT *cur) {
  auto pastEnd = start + len;
  if (cur == pastEnd) {
    return start;
  } else if (cur > pastEnd) {
    abort();
  }
  return cur;
}

static void _queueEnqueue(_Queue *queue, std::string element) {
  if (_queueLength(queue) >= (queue->size - 1)) {
    // buffer full
    queue->start += 1;
    if (queue->start == queue->contents + queue->size) {
      queue->start = queue->contents;
    }
  }
  *queue->next = element;
  queue->next += 1;
  if (queue->next == queue->contents + queue->size) {
    queue->next = queue->contents;
  }
}

static void _queueRenderLines(_Queue *queue, int y, int x, int _, int width) {
  // TODO check if height < size
  int currentY = y;
  size_t len = _queueLength(queue);
  auto ptr = queue->start;
  for (size_t i = 0; i < len; i++, ptr++) {
    ptr = _maybeWrap(queue->size, queue->contents, ptr);
    mvaddnstr(currentY, x, ptr->c_str(), width);
    currentY++;
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

  _queueRenderLines(&dbg->debugQueue, y + 1, x + 1, height - 2, width - 2);
}

void _renderInstruction(Debugger *dbg) {
  constexpr int x = 0;
  constexpr int y = 4;
  constexpr int width = 30;
  // TODO take a height as an argument
  const int height = dbg->instructionQueue.size + 2;
  _renderBox(y, x, height, width);
  _queueRenderLines(&dbg->instructionQueue, y + 1, x + 1, height - 2,
                    width - 2);
}

void _renderRegisters(Debugger *dbg) {
  constexpr int x = 0;
  constexpr int y = 0;
  constexpr int width = 27;
  constexpr int height = 4;
  VM *vm = (VM *)dbg;
  _renderBox(y, x, height, width);
  mvprintw(y + 1, x + 1, "PC   A  X  Y  SP NV-BDIZC");
  mvprintw(y + 2, x + 1, "%04X %02X %02X %02X %02X %s", wordTo16(&vm->PC),
           vm->A, vm->X, vm->Y, vm->SP,
           std::bitset<8>{vm->S}.to_string().data());
}

void _renderStack(Debugger *dbg) {
  constexpr int x = 30;
  constexpr int y = 0;
  constexpr int width = 10;
  constexpr int height = 13;
  VM *vm = (VM *)dbg;
  for (int i = 0; i < height; i++) {
    uint16_t ptr = vm->SP + i + 0x0100;
    if (ptr > 0x01FF) {
      break;
    }
    auto val = vmPeek16(vm, ptr);
    mvprintw(y + i + 1, x + 1, "%04X: %02X", ptr, val);
  }
  _renderBox(y, x, height, width);
}

extern void debuggerDebug(VM *vm, const char *s) {
  auto debugger = (Debugger *)vm;
  _queueEnqueue(&debugger->debugQueue, s);
}

Debugger debuggerNew(Rom *rom) {
  constexpr size_t instructionLen = 5;
  constexpr size_t debugLen = 30;
  setlocale(LC_ALL, "en_US.UTF-8");
  initscr();
  auto instructionBuffer = new std::string[instructionLen];
  auto debugBuffer = new std::string[debugLen];
  return Debugger{
      .super = vmNew(rom, debuggerDebug),
      .instructionQueue =
          _Queue{
              .size = instructionLen,
              .contents = instructionBuffer,
              .start = instructionBuffer,
              .next = instructionBuffer,
          },
      .debugQueue =
          _Queue{
              .size = debugLen,
              .contents = debugBuffer,
              .start = debugBuffer,
              .next = debugBuffer,
          },
  };
}

void debuggerDispose(Debugger *debugger) {
  printw("about to call endwin()\n");
  endwin();
  printf("called endwin()\n");
  auto it = debugger->debugQueue.start;
  auto len = _queueLength(&debugger->debugQueue);
  for (size_t i = 0; i < len; i++, it++) {
    _maybeWrap(debugger->debugQueue.size, debugger->debugQueue.contents, it);
    printf("%ld: %s\n", i, it->c_str());
  }
}

void debuggerRender(Debugger *debugger) {
  clear();
  _renderInstruction(debugger);
  _renderRegisters(debugger);
  _renderStack(debugger);
  _renderDebug(debugger);

  // prompt
  mvaddstr(debugger->instructionQueue.size + 6, 0, "> ");
  refresh();
}

void debuggerStart(Debugger *debugger) {
  VM *vm = (VM *)debugger;
  vm->PC = Word{
      .high = vmPeek16(vm, 0xFFFD),
      .low = vmPeek16(vm, 0xFFFC),
  };

  while (1) {
    char *msg = nullptr;
    resultTry(chrisprintf(&msg, "PC = %02X%02X", vm->PC.high, vm->PC.low));
    vm->debug(vm, msg);
    auto insLoc = vm->PC;
    Instruction ins = vmDecodeInstruction(vm);
    resultTry(chrisprintf(&msg, "%04X: %s", wordTo16(&insLoc), instructionToString(&ins)));
    _queueEnqueue(&debugger->instructionQueue,
                  std::format("{:4X}: {}", wordTo16(&insLoc),
                              instructionToString(&ins).data()));
    vmExecute(vm, ins);
    debuggerRender(debugger);

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
      vm->ppuRegisters[2] = 1 << 7;
      vm->debug(vm,
                std::format("Setting PPU[2] = #{:02X}", vm->ppuRegisters[2]));
      continue;
    } else if ((strncmp(inputLine, "exit", 4)) ||
               strncmp(inputLine, "quit", 4)) {
      throw std::runtime_error("Exit");
    } else {
      throw std::runtime_error(
          std::format("Unrecognized debugger input: \"{}\" ({})", inputLine,
                      strlen(inputLine)));
    }
  }
}
