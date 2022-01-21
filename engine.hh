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
  void evalBody(const std::vector<Expression> &body);
  void evalExpression(const Expression &expression);

public:
  ~Engine();

  void eval(std::istream &source);
};

#endif // ENGINE_HH
