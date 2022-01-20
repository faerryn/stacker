#include "engine.hh"

#include "parser.hh"
#include <cstdlib>
#include <cstring>
#include <iostream>

std::int64_t boolToInt64(bool b) { return b ? ~0 : 0; }
bool int64ToBool(std::int64_t i) { return i != 0; }

void Engine::Stack::push(std::int64_t number) { data.push_back(number); }

std::int64_t Engine::Stack::pop() {
  if (data.empty()) {
    std::cerr << __FILE__ << ":" << __LINE__ << ": empty stack\n";
    exit(EXIT_FAILURE);
  }
  const std::int64_t result = data.back();
  data.pop_back();
  return result;
}

bool Engine::Stack::empty() { return data.empty(); }

void Engine::Stack::debug() {
  std::cout << "<" << data.size() << "> ";
  for (const std::int64_t number : data) {
    std::cout << number << " ";
  }
}

void Engine::evalBody(const std::vector<Expression> &body) {
  for (const Expression &expr : body) {
    evalExpression(expr);
  }
}

void Engine::define(const std::string &word,
                    const std::vector<Expression> &def) {
  if (dictionary.contains(word)) {
    std::cerr << __FILE__ << ":" << __LINE__ << ": word already defined\n";
    exit(EXIT_FAILURE);
  }
  dictionary[word] = def;
}

void Engine::eval(std::istream &is) {
  const std::optional<Expression> &expression = parse(is);
  if (expression) {
    evalExpression(*expression);
  }
}

void Engine::evalExpression(const Expression &expression) {
  switch (expression.type) {

  case Expression::Type::Number:
    parameterStack.push(std::get<std::int64_t>(expression.data));
    break;
  case Expression::Type::Word: {
    const std::string &word = std::get<std::string>(expression.data);
    const auto &find = dictionary.find(word);
    if (find != dictionary.end()) {
      Stack returnStackMove = std::move(returnStack);
      returnStack = Stack();
      const std::vector<Expression> &body = find->second;
      evalBody(body);
      if (!returnStack.empty()) {
        std::cerr << __FILE__ << ":" << __LINE__
                  << ": expected empty return stack\n";
        exit(EXIT_FAILURE);
      }
      returnStack = std::move(returnStackMove);
    } else {
      std::cerr << __FILE__ << ":" << __LINE__ << ": unknown word\n";
      exit(EXIT_FAILURE);
    }
  } break;
  case Expression::Type::String: {
    const std::string &str = std::get<std::string>(expression.data);
    std::uint8_t *const addr = new std::uint8_t[str.size()];
    allocs.insert(addr);
    std::memcpy(addr, str.data(), str.size());
    parameterStack.push(reinterpret_cast<std::int64_t>(addr));
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

  case Expression::Type::Define: {
    const Expression::Def &def = std::get<Expression::Def>(expression.data);
    define(def.word, def.body);
  } break;

  case Expression::Type::IfThen: {
    const std::vector<Expression> &body =
        std::get<std::vector<Expression>>(expression.data);
    if (int64ToBool(parameterStack.pop())) {
      evalBody(body);
    }
  } break;
  case Expression::Type::IfElseThen: {
    const Expression::IfElse &ifElse =
        std::get<Expression::IfElse>(expression.data);
    if (int64ToBool(parameterStack.pop())) {
      evalBody(ifElse.ifBody);
    } else {
      evalBody(ifElse.elseBody);
    }
  } break;

  case Expression::Type::BeginUntil: {
    const std::vector<Expression> &body =
        std::get<std::vector<Expression>>(expression.data);
    do {
      evalBody(body);
    } while (!int64ToBool(parameterStack.pop()));
  } break;
  case Expression::Type::BeginWhileRepeat: {
    const Expression::BeginWhile &beginWhile =
        std::get<Expression::BeginWhile>(expression.data);
    evalBody(beginWhile.condBody);
    while (int64ToBool(parameterStack.pop())) {
      evalBody(beginWhile.whileBody);
      evalBody(beginWhile.condBody);
    }
  } break;
  case Expression::Type::BeginAgain: {
    const std::vector<Expression> &body =
        std::get<std::vector<Expression>>(expression.data);
    while (true) {
      evalBody(body);
    }
  } break;

  case Expression::Type::Store: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    *reinterpret_cast<std::int64_t *>(b) = a;
  } break;
  case Expression::Type::Fetch:
    parameterStack.push(
        *reinterpret_cast<std::int64_t *>(parameterStack.pop()));
    break;
  case Expression::Type::CStore: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    *reinterpret_cast<char *>(b) = char(a);
  } break;
  case Expression::Type::CFetch:
    parameterStack.push(*reinterpret_cast<char *>(parameterStack.pop()));
    break;
  case Expression::Type::Alloc: {
    const std::int64_t size = parameterStack.pop();
    if (size <= 0) {
      std::cerr << "expected positive alloc\n";
      exit(EXIT_FAILURE);
    }
    std::uint8_t *const addr = new std::uint8_t[size];
    allocs.insert(addr);
    parameterStack.push(reinterpret_cast<std::int64_t>(addr));
  } break;
  case Expression::Type::Free: {
    std::uint8_t *const addr =
        reinterpret_cast<std::uint8_t *>(parameterStack.pop());
    if (allocs.contains(addr)) {
      allocs.erase(addr);
      delete[] addr;
    } else {
      std::cerr << __FILE__ << ":" << __LINE__ << "improper free\n";
      exit(EXIT_FAILURE);
    }
  } break;

  case Expression::Type::DotS:
    parameterStack.debug();
    break;
  case Expression::Type::Bye:
    exit(EXIT_SUCCESS);
    break;
  }
}

Engine::~Engine() {
  if (!allocs.empty()) {
    std::cerr << __FILE__ << ":" << __LINE__ << ": found memory leak\n";
    exit(EXIT_FAILURE);
  }
  if (!returnStack.empty()) {
    std::cerr << __FILE__ << ":" << __LINE__
              << ": expected empty return stack\n";
    exit(EXIT_FAILURE);
  }
}
