#ifndef ENGINE_HH
#define ENGINE_HH

#include <cstdint>
#include <map>
#include <stack>
#include <string>

#include "parser.hh"

class Engine {
private:
  class Stack {
  private:
    std::stack<std::int64_t> data;
  public:
    void push(std::int64_t number);
    std::int64_t pop();
  };
  Stack parameterStack;
  Stack returnStack;
  std::map<std::string, Expression::Body> dictionary;

  void evalBody(const Expression::Body &body);
public:
  void eval(const Expression &expression);
};

#endif // ENGINE_HH
