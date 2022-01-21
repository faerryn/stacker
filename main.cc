#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "engine.hh"
#include "compiler.hh"

Engine engine;
Compiler compiler;

void evalFile(const std::filesystem::path &path) {
  std::ifstream file{path};
  if (!file.is_open()) {
    std::cerr << __FILE__ << ":" << __LINE__ << path
              << ": : No such file or directory\n";
    exit(EXIT_FAILURE);
  }
  engine.eval(file);
  file.close();
}

void compileFile(const std::filesystem::path &path) {
  std::ifstream file{path};
  if (!file.is_open()) {
    std::cerr << __FILE__ << ":" << __LINE__ << path
              << ": : No such file or directory\n";
    exit(EXIT_FAILURE);
  }

  compiler.compile(file);
  file.close();
}

int main(int argc, char **argv) {
  std::filesystem::path selfPath{argv[0]};
  std::filesystem::path corePath = selfPath.replace_filename("core.forth");
  evalFile(corePath);

  if (argc >= 2) {
    std::filesystem::path sourcePath{argv[1]};

    evalFile(sourcePath);
    compileFile(corePath);
    compileFile(argv[1]);

    std::filesystem::path destinationPath = sourcePath.concat(".cc");
    std::ofstream destination(destinationPath);
    compiler.write(destination);
    destination.close();
  } else {
    engine.eval(std::cin);
  }

  return EXIT_SUCCESS;
}
