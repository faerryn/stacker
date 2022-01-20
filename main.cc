#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>

#include "engine.hh"
#include "parser.hh"

int main(int argc, char **argv) {
  Engine engine;
  for (int i = 1; i < argc; ++i) {
    std::ifstream file{argv[i]};
    if (!file.is_open()) {
      std::cerr << argv[i] << ": No such file or directory\n";
      exit(EXIT_FAILURE);
    }
    LexemeSource source(&file);
    std::optional<Expression> expr;
    while ((expr = parse(source))) {
      engine.eval(*expr);
    }
    file.close();
  }
  return EXIT_SUCCESS;
}
