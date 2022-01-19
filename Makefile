CXXFLAGS ?= -g
CXXFLAGS += -std=c++20 -Werror -Wall -Wextra -Wpedantic

sources := main.cc lexer.cc parser.cc engine.cc
objects := $(patsubst %.cc,%.o,$(sources))
depends := $(patsubst %.cc,%.d,$(sources))

.PHONY: all clean

stacker: $(objects)
	$(CXX) $(CXXFLAGS) $^ -o $@

all: stacker

clean:
	$(RM) $(objects) $(depends) stacker

-include $(depends)

%.o: %.cc Makefile
	$(CXX) $(CXXFLAGS) -MD -MP -c $< -o $@
