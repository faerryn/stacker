CXXFLAGS ?= -g
CXXFLAGS += -std=c++20 -Werror -Wall -Wextra -Wpedantic
objects = stacker.o

stacker: $(objects)
	$(CXX) $(CXXFLAGS) -o stacker $(objects)

.PHONY: clean
clean:
	rm -f stacker $(objects)
