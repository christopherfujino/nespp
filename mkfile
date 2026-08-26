# TODO make clang
CC = clang++
CFLAGS = -g -std=c++20 -Wall -Werror -Wpedantic -Wextra -I./include
LDFLAGS = -lncursesw

# TODO: add depfiles

text-debugger.exe: \
    bin/text-debugger.o \
    lib/rom.o \
    lib/debug.o \
    lib/vm.o \
    lib/word.o \
    lib/instructions.o
  $CC $LDFLAGS $prereq -o $target

bin/%.o: bin/%.cpp
  $CC $CFLAGS -c bin/$stem.cpp -o $target

lib/%.o: lib/%.cpp
  $CC $CFLAGS -c lib/$stem.cpp -o $target

clean:V:
  rm -rf **/*.d **/*.o **/*.a *.exe
