CXXFLAGS ?= -g
CXXFLAGS += -std=c++20 -Werror -Wall -Wextra -Wpedantic
objects = main.o lexer.o parser.o engine.o

stacker: $(objects)
	$(CXX) $(CXXFLAGS) -o stacker $(objects)

.PHONY: clean
clean:
	rm -f stacker $(objects)
