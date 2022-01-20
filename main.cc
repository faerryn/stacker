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
  if (argc == 1) {
    engine.evalIStream(std::cin);
  }
  for (int i = 1; i < argc; ++i) {
    if (std::strcmp(argv[i], "-") == 0) {
      engine.evalIStream(std::cin);
    } else {
      evalFile(argv[i]);
    }
  }
  return EXIT_SUCCESS;
}
