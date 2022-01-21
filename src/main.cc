#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "compiler.hh"
#include "engine.hh"

bool evalFile(Engine &engine, const std::filesystem::path &path) {
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

void compileFile(Compiler &compiler, const std::filesystem::path &path) {
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

  if (argc == 1) {
    std::vector<const char *> args;
    for (int i = 0; i < argc; ++i) {
      args.push_back(argv[i]);
    }
    Engine engine;
    evalFile(engine, corePath);

    engine.pushArgs(args);
    engine.eval(std::cin);
  } else if (argc >= 3) {
    const std::filesystem::path sourcePath{argv[2]};
    if (std::strcmp(argv[1], "interp") == 0) {
      std::vector<const char *> args;
      for (int i = 2; i < argc; ++i) {
        args.push_back(argv[i]);
      }
      Engine engine;
      evalFile(engine, corePath);

      engine.pushArgs(args);
      const bool flag = evalFile(engine, sourcePath);
      if (flag) {
        engine.eval(std::cin);
      }
    } else if (std::strcmp(argv[1], "comp") == 0) {
      Compiler compiler;
      compileFile(compiler, corePath);
      compileFile(compiler, argv[2]);

      std::filesystem::path destinationPath = sourcePath;
      destinationPath.concat(".cc");

      std::ofstream destination(destinationPath);
      compiler.write(destination);
      destination.close();
    } else {
      std::cerr << "unknown command " << argv[1] << "\n";
    }
  } else {
    std::cerr << "unknown command " << argv[1] << "\n";
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
