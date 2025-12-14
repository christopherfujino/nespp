cmake_minimum_required(VERSION 3.31)

project(xsynth-cmake-no-projucer
  LANGUAGES CXX
  VERSION 0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)

# project global
add_compile_options(-Wall -Werror -Wpedantic)

add_executable(text-debugger
  bin/text-debugger.cpp
  lib/debug.cpp)

target_link_libraries(text-debugger vm)

# Main code
add_library(vm
  lib/address.cpp
  lib/instructions.cpp
  lib/rom.cpp
  lib/vm.cpp)
