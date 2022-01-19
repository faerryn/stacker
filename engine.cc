#include "engine.hh"

#include <cstdio>

#include "parser.hh"

void Engine::Stack::push(std::int64_t number) { data.push(number); }

std::int64_t Engine::Stack::pop() {
  if (data.empty()) {
    fprintf(stderr, "empty stack\n");
    exit(EXIT_FAILURE);
  }
  const std::int64_t result = data.top();
  data.pop();
  return result;
}

void Engine::evalBody(const Expression::Body &body) {
  for (const Expression &expr : body) {
    eval(expr);
  }
}

std::int64_t boolToInt64(bool b) { return b ? ~0 : 0; }

bool int64ToBool(std::int64_t i) { return i != 0; }

void Engine::eval(const Expression &expression) {
  switch (expression.type) {
  case Expression::Type::NUM:
    parameterStack.push(std::get<std::int64_t>(expression.data));
    break;
  case Expression::Type::ADD:
    parameterStack.push(parameterStack.pop() + parameterStack.pop());
    break;
  case Expression::Type::SUB:
    parameterStack.push(parameterStack.pop() - parameterStack.pop());
    break;
  case Expression::Type::MUL:
    parameterStack.push(parameterStack.pop() * parameterStack.pop());
    break;
  case Expression::Type::DIV:
    parameterStack.push(parameterStack.pop() / parameterStack.pop());
    break;
  case Expression::Type::REM:
    parameterStack.push(parameterStack.pop() % parameterStack.pop());
    break;
  case Expression::Type::MOD: {
    const std::int64_t dividend = parameterStack.pop();
    const std::int64_t divisor = parameterStack.pop();
    const std::int64_t modulus = (dividend % divisor + divisor) % divisor;
    parameterStack.push(modulus);
  } break;
  case Expression::Type::GT:
    parameterStack.push(boolToInt64(parameterStack.pop() > parameterStack.pop()));
    break;
  case Expression::Type::LT:
    parameterStack.push(boolToInt64(parameterStack.pop() < parameterStack.pop()));
    break;
  case Expression::Type::EQ:
    parameterStack.push(boolToInt64(parameterStack.pop() == parameterStack.pop()));
    break;
  case Expression::Type::NEQ:
    parameterStack.push(boolToInt64(parameterStack.pop() != parameterStack.pop()));
    break;
  case Expression::Type::AND:
    parameterStack.push(parameterStack.pop() & parameterStack.pop());
    break;
  case Expression::Type::OR:
    parameterStack.push(parameterStack.pop() | parameterStack.pop());
    break;
  case Expression::Type::INVERT:
    parameterStack.push(~parameterStack.pop());
    break;
  case Expression::Type::DOT:
    printf("%ld ", parameterStack.pop());
    break;
  case Expression::Type::EMIT:
    printf("%c", char(parameterStack.pop()));
    break;
  case Expression::Type::DUP: {
    const std::int64_t top = parameterStack.pop();
    parameterStack.push(top);
    parameterStack.push(top);
  } break;
  case Expression::Type::DROP:
    parameterStack.pop();
    break;
  case Expression::Type::SWAP: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(b);
    parameterStack.push(a);
  } break;
  case Expression::Type::OVER: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a);
    parameterStack.push(b);
    parameterStack.push(a);
  } break;
  case Expression::Type::ROT: {
    const std::int64_t c = parameterStack.pop();
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(b);
    parameterStack.push(c);
    parameterStack.push(a);
  } break;
  case Expression::Type::RPUT:
    returnStack.push(parameterStack.pop());
    break;
  case Expression::Type::RGET:
    parameterStack.push(returnStack.pop());
    break;
  case Expression::Type::WORD: {
    const std::string &word = std::get<std::string>(expression.data);
    const auto &find = dictionary.find(word);
    if (find != dictionary.end()) {
      const Expression::Body &body = find->second;
      for (const Expression &expr : body) {
        eval(expr);
      }
    } else {
      fprintf(stderr, "unknown word\n");
      exit(EXIT_FAILURE);
    }
  } break;
  case Expression::Type::DEF: {
    const Expression::Def &def = std::get<Expression::Def>(expression.data);
    dictionary[def.word] = def.body;
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
  }
}
