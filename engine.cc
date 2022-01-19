#include "engine.hh"
#include "parser.hh"

void Engine::push(std::int64_t number) { dataStack.push(number); }

std::int64_t Engine::pop() {
  if (dataStack.empty()) {
    fprintf(stderr, "empty stack\n");
    exit(EXIT_FAILURE);
  }
  const std::int64_t result = dataStack.top();
  dataStack.pop();
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
    push(std::get<std::int64_t>(expression.data));
    break;
  case Expression::Type::ADD:
    push(pop() + pop());
    break;
  case Expression::Type::SUB:
    push(pop() - pop());
    break;
  case Expression::Type::MUL:
    push(pop() * pop());
    break;
  case Expression::Type::DIV:
    push(pop() / pop());
    break;
  case Expression::Type::REM:
    push(pop() % pop());
    break;
  case Expression::Type::MOD: {
    const std::int64_t dividend = pop();
    const std::int64_t divisor = pop();
    const std::int64_t modulus = (dividend % divisor + divisor) % divisor;
    push(modulus);
  } break;
  case Expression::Type::GT:
    push(boolToInt64(pop() > pop()));
    break;
  case Expression::Type::LT:
    push(boolToInt64(pop() < pop()));
    break;
  case Expression::Type::EQ:
    push(boolToInt64(pop() == pop()));
    break;
  case Expression::Type::NEQ:
    push(boolToInt64(pop() != pop()));
    break;
  case Expression::Type::AND:
    push(pop() & pop());
    break;
  case Expression::Type::OR:
    push(pop() | pop());
    break;
  case Expression::Type::INVERT:
    push(~pop());
    break;
  case Expression::Type::DOT:
    printf("%ld ", pop());
    break;
  case Expression::Type::EMIT:
    printf("%c", char(pop()));
    break;
  case Expression::Type::DUP: {
    const std::int64_t top = pop();
    push(top);
    push(top);
  } break;
  case Expression::Type::DROP:
    pop();
    break;
  case Expression::Type::SWAP: {
    const std::int64_t b = pop();
    const std::int64_t a = pop();
    push(b);
    push(a);
  } break;
  case Expression::Type::OVER: {
    const std::int64_t b = pop();
    const std::int64_t a = pop();
    push(a);
    push(b);
    push(a);
  } break;
  case Expression::Type::ROT: {
    const std::int64_t c = pop();
    const std::int64_t b = pop();
    const std::int64_t a = pop();
    push(b);
    push(c);
    push(a);
  } break;
  case Expression::Type::WORD: {
    const std::string &word = std::get<std::string>(expression.data);
    if (const auto &find = defDict.find(word); find != defDict.end()) {
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
    defDict[def.word] = def.body;
  } break;
  case Expression::Type::IF: {
    const Expression::Body &body = std::get<Expression::Body>(expression.data);
    if (int64ToBool(pop())) {
      evalBody(body);
    }
  } break;
  case Expression::Type::IF_ELSE: {
    const Expression::IfElse &ifElse =
        std::get<Expression::IfElse>(expression.data);
    if (int64ToBool(pop())) {
      evalBody(ifElse.ifBody);
    } else {
      evalBody(ifElse.elseBody);
    }
  } break;
  case Expression::Type::BEGIN: {
    const Expression::Body &body = std::get<Expression::Body>(expression.data);
    do {
      evalBody(body);
    } while (!int64ToBool(pop()));
  } break;
  case Expression::Type::BEGIN_WHILE: {
    const Expression::BeginWhile &beginWhile =
        std::get<Expression::BeginWhile>(expression.data);
    evalBody(beginWhile.condBody);
    while (int64ToBool(pop())) {
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
