CXXFLAGS ?= -g
override CXXFLAGS += -std=c++20 -Werror -Wall -Wextra -Wpedantic

SOURCES := src/main.cc src/lexer.cc src/parser.cc src/engine.cc src/compiler.cc
OBJECTS := $(patsubst %.cc,%.o,$(SOURCES))
DEPENDS := $(patsubst %.cc,%.d,$(SOURCES))


stacker: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

-include $(depends)

%.o: %.cc Makefile
	$(CXX) $(CXXFLAGS) -MD -MP -c $< -o $@

.PHONY: all clean

all: stacker

clean:
	$(RM) $(OBJECTS) $(DEPENDS) stacker
