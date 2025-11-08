RAYLIB_VERSION = 5.5
RAYLIB_INCLUDE = ./THIRD_PARTY/raylib-$(RAYLIB_VERSION)/out/raylib/include
RAYLIB_BUILD = ./THIRD_PARTY/raylib-$(RAYLIB_VERSION)/out/raylib
#LDFLAGS = -L$(RAYLIB_BUILD)
BUILD = ./build
BINARY = $(BUILD)/a.out
BIN_DIR = ./bin
LIB_IR = ./lib
INCLUDE_DIR = ./include
INCLUDES = -I$(RAYLIB_INCLUDE) -I$(INCLUDE_DIR)
MAKEFILE_WARNINGS ?= -Wall -Werror -Wpedantic
CXXFLAGS = -g -std=c++20 $(MAKEFILE_WARNINGS) $(INCLUDES)
CXX = clang++

.PHONY: headless
headless: $(BUILD)/main rom.nes
	$< ./rom.nes

.PHONY: run
run: $(BUILD)/tileBrowser rom.nes
	$< ./rom.nes

.PHONY: all
all: $(BUILD)/tileBrowser $(BUILD)/main
	@echo "Done"

$(BUILD)/tileBrowser: $(BUILD)/tileBrowser.o $(BUILD)/rom.o $(RAYLIB_BUILD)/libraylib.a
	$(CXX) $(LDFLAGS) $(BUILD)/tileBrowser.o $(BUILD)/rom.o $(RAYLIB_BUILD)/libraylib.a -o $@

# Headless
$(BUILD)/main: $(BUILD)/main.o $(BUILD)/rom.o $(BUILD)/instructions.o $(BUILD)/debug.o $(BUILD)/vm.o
	$(CXX) $^ -o $@

$(BUILD)/rom.o: lib/rom.cpp $(INCLUDE_DIR)/rom.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/tileBrowser.o: $(BIN_DIR)/tileBrowser.cpp $(INCLUDE_DIR)/rom.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/main.o: $(BIN_DIR)/main.cpp $(INCLUDE_DIR)/rom.h $(INCLUDE_DIR)/vm.h $(INCLUDE_DIR)/address.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/instructions.o: lib/instructions.cpp $(INCLUDE_DIR)/instructions.h $(INCLUDE_DIR)/debug.h $(INCLUDE_DIR)/address.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/debug.o: lib/debug.cpp $(INCLUDE_DIR)/debug.h $(INCLUDE_DIR)/instructions.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/vm.o: lib/vm.cpp $(INCLUDE_DIR)/vm.h $(INCLUDE_DIR)/address.h $(INCLUDE_DIR)/rom.h $(INCLUDE_DIR)/instructions.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

rom.nes:
	curl -L http://nickmass.com/images/nestest.nes -o ./rom.nes

.PHONY: clean
clean:
	rm $(BUILD)/*
