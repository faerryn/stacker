CXXFLAGS ?= -g
CXXFLAGS += -std=c++20 -Werror -Wall -Wextra -Wpedantic

sources := $(wildcard *.cc)
objects := $(patsubst %.cc,%.o,$(sources))
depends := $(patsubst %.cc,%.d,$(sources))


stacker: $(objects)
	$(CXX) $(CXXFLAGS) $^ -o $@

-include $(depends)

%.o: %.cc Makefile
	$(CXX) $(CXXFLAGS) -MD -MP -c $< -o $@

.PHONY: all clean

all: stacker

clean:
	$(RM) $(objects) $(depends) stacker
