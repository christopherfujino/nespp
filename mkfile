PROJECT = nespp
CC = bear --append -- clang
CXX = bear --append -- clang++
AR = llvm-ar
DEBUG_FLAGS = -g -O0 #-fsanitize=address
CXXFLAGS = $DEBUG_FLAGS -std=c++20 -Wall -Werror -Wpedantic -Wextra -I./include
CFLAGS = $DEBUG_FLAGS -std=c23 -Wall -Werror -Wpedantic -Wextra -I./include
LDFLAGS = #-fsanitize=address
LD_CURSES = -lncursesw
DEPFILES = `{/bin/sh -c 'find . -name "*.d"'}
RAYLIB_VERSION = 5.5
RAYLIB_INCLUDE = ../../3p/raylib/build/raylib/include
RAYLIB_BUILD = ../../3p/raylib/build/raylib

all:V: text-debugger.exe tileBrowser.exe romToSource.exe

text-debugger.exe: bin/text-debugger.o lib/lib$PROJECT.a
  $CXX $LD_CURSES $LDFLAGS $prereq -o $target

<|cat $DEPFILES /dev/null

tileBrowser.exe: bin/tileBrowser.o lib/lib$PROJECT.a $RAYLIB_BUILD/libraylib.a
	$CXX $LDFLAGS -lm -lX11 $prereq -o $target

romToSource.exe: bin/romToSource.o lib/lib$PROJECT.a $RAYLIB_BUILD/libraylib.a
	$CXX $LDFLAGS -lm -lX11 $prereq -o $target

test:V: test.exe
  ./test.exe

test.exe: bin/test.o lib/lib$PROJECT.a
  $CC $LDFLAGS $prereq -o $target

bin/%.o: bin/%.c
  $CC $CFLAGS \
    -MT $target -MMD -MP -MF bin/$stem.d \
    -c bin/$stem.c -o $target

bin/romToSource.o: bin/romToSource.c
  $CC $CFLAGS -I$RAYLIB_INCLUDE \
    -MT $target -MMD -MP -MF bin/$stem.d \
    -c bin/romToSource.c -o $target

bin/tileBrowser.o: bin/tileBrowser.cpp
  $CXX $CXXFLAGS -I$RAYLIB_INCLUDE \
    -MT $target -MMD -MP -MF bin/$stem.d \
    -c bin/tileBrowser.cpp -o $target

bin/%.o: bin/%.cpp
  $CXX $CXXFLAGS \
    -MT $target -MMD -MP -MF bin/$stem.d \
    -c bin/$stem.cpp -o $target

lib/%.o: lib/%.c
  $CC $CFLAGS \
    -MT $target -MMD -MP -MF lib/$stem.d \
    -c lib/$stem.c -o $target

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

# vim: ft=make
