#include "engine.hh"

#include "parser.hh"

void Engine::Stack::push(std::int64_t number) { data.push(number); }

std::int64_t Engine::Stack::pop() {
  if (data.empty()) {
    std::cerr << "empty stack\n";
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

  case Expression::Type::NUM:
    parameterStack.push(std::get<std::int64_t>(expression.data));
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
      std::cerr << "unknown word\n";
      exit(EXIT_FAILURE);
    }
  } break;

  case Expression::Type::ADD: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a + b);
  } break;
  case Expression::Type::SUB: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a - b);
  } break;
  case Expression::Type::MUL: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a * b);
  } break;
  case Expression::Type::DIV: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a / b);
  } break;
  case Expression::Type::REM: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a % b);
  } break;
  case Expression::Type::MOD: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push((a % b + b) % b);
  } break;

  case Expression::Type::GT: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(boolToInt64(a > b));
  } break;
  case Expression::Type::LT: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(boolToInt64(a < b));
  } break;
  case Expression::Type::EQ: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(boolToInt64(a == b));
  } break;
  case Expression::Type::NEQ: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(boolToInt64(a != b));
  } break;

  case Expression::Type::AND: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a & b);
  } break;
  case Expression::Type::OR: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a | b);
  } break;
  case Expression::Type::INVERT:
    parameterStack.push(~parameterStack.pop());
    break;

  case Expression::Type::DOT:
    std::cout << parameterStack.pop() << " ";
    break;
  case Expression::Type::EMIT:
    std::cout << char(parameterStack.pop());
    break;

  case Expression::Type::DUP: {
    const std::int64_t a = parameterStack.pop();
    parameterStack.push(a);
    parameterStack.push(a);
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

  case Expression::Type::VARIABLE: {
    const std::string &word = std::get<std::string>(expression.data);
    define(word, {{Expression::Type::NUM, std::int64_t(tape.size())}});
    tape.push_back(0);
  } break;
  case Expression::Type::STORE: {
    const std::int64_t b = parameterStack.pop();
    const std::int64_t a = parameterStack.pop();
    tape[b] = a;
  } break;
  case Expression::Type::FETCH:
    parameterStack.push(tape[parameterStack.pop()]);
    break;
  }
}
