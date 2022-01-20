#ifndef ENGINE_HH
#define ENGINE_HH

#include <cstdint>
#include <map>
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
    void debug();
  };
  Stack parameterStack;
  Stack returnStack;
  std::map<std::string, Expression::Body> dictionary;
  std::vector<std::int64_t *> variables;

  void evalBody(const Expression::Body &body);
  void define(const std::string &word, const Expression::Body &def);

public:
  ~Engine();

  void eval(const Expression &expression);
};

#endif // ENGINE_HH
