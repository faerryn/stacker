#ifndef COMPILER_HH
#define COMPILER_HH

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "parser.hh"

class Compiler {
private:
  std::map<std::string, int> dictionary;
  int nextDictionaryName;

  std::string declarationSection;
  std::string definitionSection;
  std::string mainSection;

  void compileBody(const std::vector<Expression> &body,
                   std::string &destination);
  void compileExpression(const Expression &expression,
                         std::string &destination);

public:
  void compile(std::istream &source);
  void write(std::ostream &destination);
};

#endif // COMPILER_HH
