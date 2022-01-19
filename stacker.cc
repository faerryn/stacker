#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <optional>
#include <stack>
#include <string>
#include <variant>
#include <vector>

#include "lexer.hh"
#include "parser.hh"
#include "engine.hh"

int main(int argc, char **argv) {
  Engine engine;
  bool evalStdin = (argc == 1);
  for (int i = 1; i < argc; ++i) {
    if (std::string(argv[i]) == "-") {
      evalStdin = true;
      continue;
    }
    std::FILE *const file = fopen(argv[i], "r");
    if (file == stdin) {
      evalStdin = true;
      continue;
    }
    if (!file) {
      perror(argv[i]);
      exit(EXIT_FAILURE);
    }
    LexemeSource source(file);
    std::optional<Expression> expr;
    while ((expr = parse(source))) {
      engine.eval(*expr);
    }
    fclose(file);
  }
  if (evalStdin) {
    LexemeSource source(stdin);
    std::optional<Expression> expr;
    while ((expr = parse(source))) {
      engine.eval(*expr);
    }
  }
  return EXIT_SUCCESS;
}
