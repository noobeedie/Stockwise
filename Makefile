CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -O2

all: stockwise

stockwise: src/stockwise.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

run: stockwise
	./stockwise

clean:
	rm -f stockwise stockwise.exe

.PHONY: all run clean
