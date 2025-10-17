RAYLIB_VERSION = 5.5
RAYLIB_INCLUDE = ./THIRD_PARTY/raylib-$(RAYLIB_VERSION)/out/raylib/include
RAYLIB_BUILD = ./THIRD_PARTY/raylib-$(RAYLIB_VERSION)/out/raylib
LDFLAGS = -L$(RAYLIB_BUILD)
BUILD = ./build
BINARY = $(BUILD)/a.out
BIN_DIR = ./bin
LIB_IR = ./lib
INCLUDE_DIR = ./include
CXXFLAGS = -std=c++20 -I$(RAYLIB_INCLUDE) -I$(INCLUDE_DIR)

.PHONY: run
run: $(BUILD)/tileBrowser rom.nes
	$< ./rom.nes

.PHONY: all
all: $(BUILD)/tileBrowser $(BUILD)/main
	@echo "Done"

$(BUILD)/tileBrowser: $(BUILD)/tileBrowser.o $(BUILD)/rom.o $(RAYLIB_BUILD)/libraylib.a
	clang++ $(LDFLAGS) $(BUILD)/tileBrowser.o $(BUILD)/rom.o $(RAYLIB_BUILD)/libraylib.a -o $@

$(BUILD)/main: $(BUILD)/main.o $(BUILD)/rom.o $(RAYLIB_BUILD)/libraylib.a
	clang++ $(LDFLAGS) $(BUILD)/main.o $(BUILD)/rom.o $(RAYLIB_BUILD)/libraylib.a -o $@

$(BUILD)/rom.o: rom.cpp $(INCLUDE_DIR)/rom.hpp $(RAYLIB_INCLUDE)/raylib.h
	clang++ $(CXXFLAGS) -c $< -o $@

$(BUILD)/tileBrowser.o: $(BIN_DIR)/tileBrowser.cpp $(INCLUDE_DIR)/rom.hpp
	clang++ $(CXXFLAGS) -c $< -o $@

$(BUILD)/main.o: $(BIN_DIR)/main.cpp $(INCLUDE_DIR)/rom.hpp
	clang++ $(CXXFLAGS) -c $< -o $@

rom.nes:
	curl -L http://nickmass.com/images/nestest.nes -o ./rom.nes

.PHONY: clean
clean:
	rm $(BUILD)/*
