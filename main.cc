#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "engine.hh"
#include "parser.hh"

Engine engine;

void evalFile(const std::filesystem::path &path) {
  std::ifstream file{path};
  if (!file.is_open()) {
    std::cerr << path << ": No such file or directory\n";
    exit(EXIT_FAILURE);
  }
  engine.evalIStream(file);
  file.close();
}

int main(int argc, char **argv) {
  std::filesystem::path selfPath{argv[0]};
  std::filesystem::path corePath = selfPath.replace_filename("core.forth");
  evalFile(corePath);

  if (argc >= 2) {
    evalFile(argv[1]);
  }

  engine.evalIStream(std::cin);
  return EXIT_SUCCESS;
}
