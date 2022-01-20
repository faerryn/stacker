#include "engine.hh"

#include "parser.hh"
#include <cstdlib>
#include <iostream>

void Engine::Stack::push(std::int64_t number) { data.push_back(number); }

std::int64_t Engine::Stack::pop() {
  if (data.empty()) {
    std::cerr << "empty stack\n";
    exit(EXIT_FAILURE);
  }
  const std::int64_t result = data.back();
  data.pop_back();
  return result;
}

void Engine::Stack::debug() {
  std::cerr << "<" << data.size() << "> ";
  for (const std::int64_t number : data) {
    std::cerr << number << " ";
  }
}

void Engine::evalBody(const Expression::Body &body) {
  for (const Expression &expr : body) {
    eval(expr);
  }
}

void Engine::define(const std::string &word, const Expression::Body &def) {
  if (dictionary.contains(word)) {
    std::cerr << "word already defined\n";
    exit(EXIT_FAILURE);
  }
  dictionary[word] = def;
}

std::int64_t boolToInt64(bool b) { return b ? ~0 : 0; }

bool int64ToBool(std::int64_t i) { return i != 0; }

void Engine::eval(const Expression &expression) {
  switch (expression.type) {

  case Expression::Type::Number:
    parameterStack.push(std::get<std::int64_t>(expression.data));
    break;
  case Expression::Type::Word: {
    const std::string &word = std::get<std::string>(expression.data);
    const auto &find = dictionary.find(word);
    if (find != dictionary.end()) {
      const Expression::Body &body = find->second;
      for (const Expression &expr : body) {
        eval(expr);
      }
    } else {
      std::cerr << "unknown word\n";
      exit(EXIT_FAILURE);
    }
  } break;
  case Expression::Type::String: {
    const std::string &str = std::get<std::string>(expression.data);
    std::string *newStr = new std::string(str);
    strings.push_back(newStr);
    parameterStack.push(reinterpret_cast<std::int64_t>(newStr->data()));
    parameterStack.push(str.size());
  } break;

  case Expression::Type::Add: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a + b);
  } break;
  case Expression::Type::Sub: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a - b);
  } break;
  case Expression::Type::Mul: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a * b);
  } break;
  case Expression::Type::Div: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a / b);
  } break;
  case Expression::Type::Rem: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a % b);
  } break;
  case Expression::Type::Mod: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push((a % b + b) % b);
  } break;

  case Expression::Type::More: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(boolToInt64(a > b));
  } break;
  case Expression::Type::Less: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(boolToInt64(a < b));
  } break;
  case Expression::Type::Equal: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(boolToInt64(a == b));
  } break;
  case Expression::Type::NotEqual: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(boolToInt64(a != b));
  } break;

  case Expression::Type::And: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a & b);
  } break;
  case Expression::Type::Or: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a | b);
  } break;
  case Expression::Type::Inv:
    parameterStack.push(~parameterStack.pop());
    break;

  case Expression::Type::Emit:
    std::cout.put(char(parameterStack.pop()));
    break;

  case Expression::Type::Dup: {
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a);
    parameterStack.push(a);
  } break;
  case Expression::Type::Drop:
    parameterStack.pop();
    break;
  case Expression::Type::Swap: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(b);
    parameterStack.push(a);
  } break;
  case Expression::Type::Over: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a);
    parameterStack.push(b);
    parameterStack.push(a);
  } break;
  case Expression::Type::Rot: {
    const std::int64_t c = parameterStack.pop();
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(b);
    parameterStack.push(c);
    parameterStack.push(a);
  } break;

  case Expression::Type::ToR:
    returnStack.push(parameterStack.pop());
    break;
  case Expression::Type::RFrom:
    parameterStack.push(returnStack.pop());
    break;

  case Expression::Type::DEF: {
    const Expression::Def &def = std::get<Expression::Def>(expression.data);
    define(def.word, def.body);
  } break;

  case Expression::Type::IF: {
    const Expression::Body &body = std::get<Expression::Body>(expression.data);
    if (int64ToBool(parameterStack.pop())) {
      evalBody(body);
    }
  } break;
  case Expression::Type::IF_ELSE: {
    const Expression::IfElse &ifElse =
        std::get<Expression::IfElse>(expression.data);
    if (int64ToBool(parameterStack.pop())) {
      evalBody(ifElse.ifBody);
    } else {
      evalBody(ifElse.elseBody);
    }
  } break;

  case Expression::Type::BEGIN: {
    const Expression::Body &body = std::get<Expression::Body>(expression.data);
    do {
      evalBody(body);
    } while (!int64ToBool(parameterStack.pop()));
  } break;
  case Expression::Type::BEGIN_WHILE: {
    const Expression::BeginWhile &beginWhile =
        std::get<Expression::BeginWhile>(expression.data);
    evalBody(beginWhile.condBody);
    while (int64ToBool(parameterStack.pop())) {
      evalBody(beginWhile.whileBody);
      evalBody(beginWhile.condBody);
    }
  } break;
  case Expression::Type::BEGIN_AGAIN: {
    const Expression::Body &body = std::get<Expression::Body>(expression.data);
    while (true) {
      evalBody(body);
    }
  } break;

  case Expression::Type::STORE: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    *reinterpret_cast<std::int64_t *>(b) = a;
  } break;
  case Expression::Type::FETCH: {
    const std::int64_t *addr =
        reinterpret_cast<std::int64_t *>(parameterStack.pop());
    parameterStack.push(*addr);
  } break;
  case Expression::Type::CSTORE: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    *reinterpret_cast<char *>(b) = char(a);
  } break;
  case Expression::Type::CFETCH: {
    const char *addr = reinterpret_cast<char *>(parameterStack.pop());
    parameterStack.push(*addr);
  } break;

  case Expression::Type::DEBUG:
    parameterStack.debug();
    break;
  case Expression::Type::BYE:
    exit(EXIT_SUCCESS);
    break;
  }
}

Engine::~Engine() {
  for (const std::string *pair : strings) {
    delete pair;
  }
}
