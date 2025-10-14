CXXFLAGS = -std=c++20

.PHONY: run
run: a.out
	./a.out

a.out: main.o rom.o
	clang++ $(CXXFLAGS) main.o rom.o -o a.out

rom.o: rom.cpp rom.hpp
	clang++ $(CXXFLAGS) -c rom.cpp -o rom.o

main.o: main.cpp rom.hpp
	clang++ $(CXXFLAGS) -c main.cpp -o main.o

rom.nes:
	curl -L http://nickmass.com/images/nestest.nes -o ./rom.nes

.PHONY: clean
clean:
	rm -f *.o a.out
