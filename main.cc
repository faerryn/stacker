#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "engine.hh"

Engine engine;

void evalLoop(std::istream &is) {
  while (!is.eof()) {
    engine.eval(is);
  }
}

void evalFile(const std::filesystem::path &path) {
  std::ifstream file{path};
  if (!file.is_open()) {
    std::cerr << __FILE__ << ":" << __LINE__ << path
              << ": : No such file or directory\n";
    exit(EXIT_FAILURE);
  }
  evalLoop(file);
  file.close();
}

int main(int argc, char **argv) {
  std::filesystem::path selfPath{argv[0]};
  std::filesystem::path corePath = selfPath.replace_filename("core.forth");
  evalFile(corePath);

  if (argc >= 2) {
    evalFile(argv[1]);
  }

  evalLoop(std::cin);
  return EXIT_SUCCESS;
}
