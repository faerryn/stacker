#ifndef ENGINE_HH
#define ENGINE_HH

#include <map>
#include <stack>

#include "parser.hh"

class Engine {
private:
  std::stack<std::int64_t> dataStack;
  std::map<std::string, std::vector<Expression>> defDict;
  void push(std::int64_t number);
  std::int64_t pop();

public:
  void eval(const Expression &expression);
};

#endif // ENGINE_HH
