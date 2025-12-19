cmake_minimum_required(VERSION 3.31) # Ubuntu 25.10 version

project(nespp LANGUAGES CXX VERSION 0.1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# project global
add_compile_options(-Wall -Werror -Wpedantic -Wextra)

file(DOWNLOAD http://nickmass.com/images/nestest.nes rom.nes )

add_executable(text-debugger
  bin/text-debugger.cpp
  lib/debug.cpp)

# This is only required for the debugger
set(CURSES_NEED_WIDE TRUE) # unicode
find_package(Curses REQUIRED) # provides ${CURSES_LIBRARIES}
target_include_directories(text-debugger PRIVATE
  ${CURSES_INCLUDE_DIR})
target_link_libraries(text-debugger
  ${CURSES_LIBRARIES}
  vm)

# Main code
add_library(vm
  lib/instructions.cpp
  lib/rom.cpp
  lib/word.cpp
  lib/vm.cpp)
