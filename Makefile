CXXFLAGS = -std=c++20

.PHONY: run
run: a.out
	./a.out

a.out: main.cpp
	clang++ $(CXXFLAGS) main.cpp -o a.out
