RAYLIB_VERSION = 5.5
RAYLIB_INCLUDE = ./THIRD_PARTY/raylib-$(RAYLIB_VERSION)/out/raylib/include
RAYLIB_BUILD = ./THIRD_PARTY/raylib-$(RAYLIB_VERSION)/out/raylib
CXXFLAGS = -std=c++20 -I$(RAYLIB_INCLUDE)
LDFLAGS = -L$(RAYLIB_BUILD)

.PHONY: run
run: a.out rom.nes
	./a.out ./rom.nes

a.out: main.o rom.o $(RAYLIB_BUILD)/libraylib.a
	clang++ $(LDFLAGS) main.o rom.o $(RAYLIB_BUILD)/libraylib.a -o a.out

rom.o: rom.cpp rom.hpp $(RAYLIB_INCLUDE)/raylib.h
	clang++ $(CXXFLAGS) -c rom.cpp -o rom.o

main.o: main.cpp rom.hpp
	clang++ $(CXXFLAGS) -c main.cpp -o main.o

rom.nes:
	curl -L http://nickmass.com/images/nestest.nes -o ./rom.nes

.PHONY: clean
clean:
	rm -f *.o a.out
