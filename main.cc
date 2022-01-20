#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>

#include "engine.hh"
#include "parser.hh"

int main(int argc, char **argv) {
  Engine engine;

  if (argc == 1) {
    engine.evalIStream(std::cin);
  }
  for (int i = 1; i < argc; ++i) {
    if (std::strcmp(argv[i], "-") == 0) {
      engine.evalIStream(std::cin);
    } else {
      std::ifstream file{argv[i]};
      if (!file.is_open()) {
        std::cerr << argv[i] << ": No such file or directory\n";
        exit(EXIT_FAILURE);
      }
      engine.evalIStream(file);
      file.close();
    }
  }
  return EXIT_SUCCESS;
}
