#ifndef ENGINE_HH
#define ENGINE_HH

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "parser.hh"

class Engine {
private:
  class Stack {
  private:
    std::vector<std::int64_t> data;

  public:
    void push(std::int64_t number);
    std::int64_t pop();
    bool empty();
    void debug();
  };
  Stack parameterStack;
  Stack returnStack;
  std::map<std::string, std::vector<Expression>> dictionary;
  std::set<std::uint8_t *> allocs;

  void define(const std::string &word, const std::vector<Expression> &def);
  bool evalBody(const std::vector<Expression> &body);
  bool evalExpression(const Expression &expression);

public:
  ~Engine();

  bool eval(std::istream &source);
};

#endif // ENGINE_HH
