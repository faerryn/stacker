#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "compiler.hh"
#include "engine.hh"

Engine engine;
Compiler compiler;

bool evalFile(const std::filesystem::path &path) {
  std::ifstream file{path};
  if (!file.is_open()) {
    std::cerr << __FILE__ << ":" << __LINE__ << path
              << ": : No such file or directory\n";
    exit(EXIT_FAILURE);
  }
  const bool flag = engine.eval(file);
  file.close();
  return flag;
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
  const std::filesystem::path selfPath{argv[0]};
  std::filesystem::path corePath = selfPath;
  corePath.replace_filename("core.forth");

  if (argc >= 3) {
    const std::filesystem::path sourcePath{argv[2]};
    if (std::strcmp(argv[1], "interp") == 0) {
      evalFile(corePath);
      const bool flag = evalFile(sourcePath);
      if (flag) {
        engine.eval(std::cin);
      }
    } else if (std::strcmp(argv[1], "comp") == 0) {
      compileFile(corePath);
      compileFile(argv[2]);

      std::filesystem::path ccPath = sourcePath;
      ccPath.concat(".cc");

      std::ofstream destination(ccPath);
      compiler.write(destination);
      destination.close();
    }
  } else if (argc == 1) {
    evalFile(corePath);
    engine.eval(std::cin);
  } else {
    std::cerr << "unknown command " << argv[1] << "\n";
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
