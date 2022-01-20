#ifndef ENGINE_HH
#define ENGINE_HH

#include <cstdint>
#include <map>
#include <deque>
#include <string>

#include "parser.hh"

class Engine {
private:
  class Stack {
  private:
    std::deque<std::int64_t> data;

  public:
    void push(std::int64_t number);
    std::int64_t pop();
    void debug();
  };
  Stack parameterStack;
  Stack returnStack;
  std::map<std::string, Expression::Body> dictionary;
  std::vector<std::int64_t> tape;

  void evalBody(const Expression::Body &body);
  void define(const std::string &word, const Expression::Body &def);

public:
  void eval(const Expression &expression);
  void evalIStream(std::istream &is);
};

#endif // ENGINE_HH
