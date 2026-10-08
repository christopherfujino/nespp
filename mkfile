PROJECT = nespp
CC = bear --append -- clang
CXX = bear --append -- clang++
AR = llvm-ar
DEBUG_FLAGS = -g -O0 #-fsanitize=address
CXXFLAGS = $DEBUG_FLAGS -std=c++20 -Wall -Werror -Wpedantic -Wextra -I./include
CFLAGS = $DEBUG_FLAGS -std=c23 -Wall -Werror -Wpedantic -Wextra -I./include
LDFLAGS = -lncursesw #-fsanitize=address
DEPFILES = `{/bin/sh -c 'find . -name "*.d"'}

text-debugger.exe: bin/text-debugger.o lib/lib$PROJECT.a
  $CXX $LDFLAGS $prereq -o $target

<|cat $DEPFILES /dev/null

test:V: test.exe
  ./test.exe

test.exe: bin/test.o lib/lib$PROJECT.a
  $CC $LDFLAGS $prereq -o $target

bin/%.o: bin/%.c
  $CC $CFLAGS \
    -MT $target -MMD -MP -MF bin/$stem.d \
    -c bin/$stem.c -o $target

bin/%.o: bin/%.cpp
  $CXX $CXXFLAGS \
    -MT $target -MMD -MP -MF bin/$stem.d \
    -c bin/$stem.cpp -o $target

lib/%.o: lib/%.cpp
  $CXX $CXXFLAGS \
    -MT $target -MMD -MP -MF lib/$stem.d \
    -c lib/$stem.cpp -o $target

lib/lib$PROJECT.a: \
    lib/rom.o \
    lib/debug.o \
    lib/vm.o \
    lib/word.o \
    lib/instructions.o
	$AR rcs $target $prereq

clean:V:
  rm -rf **/*.d **/*.o **/*.a *.exe
