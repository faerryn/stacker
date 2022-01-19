#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <optional>
#include <stack>
#include <string>
#include <variant>
#include <vector>

struct Lexeme {
  enum class Type {
    NUM,
    WORD,

    ADD,
    SUB,
    MUL,
    DIV,
    REM,
    MOD,

    GT,
    LT,
    EQ,

    NEQ,

    DOT,
    EMIT,

    DUP,
    DROP,
    SWITCH,
    OVER,
    ROT,

    COL,
    SEMICOL,

    IF,
    THEN,

    BEGIN,
    UNTIL,
  } type;
  std::variant<std::monostate, std::int64_t, std::string> data;
};

std::optional<std::int64_t> parseInt(const std::string &ident) {
  if (ident.empty()) {
    return {};
  }
  std::int64_t result = 0;

  std::int64_t sign = +1;
  size_t i = 0;
  if (ident[i] == '-') {
    sign = -1;
    ++i;
  } else if (ident[i] == '+') {
    sign = +1;
    ++i;
  }

  while (i < ident.size()) {
    if (std::isdigit(ident[i])) {
      result *= 10;
      result += ident[i] - '0';
    } else {
      return {};
    }
    ++i;
  }

  return sign * result;
}

std::optional<Lexeme> lexWord(const std::string &word) {
  const std::map<std::string, Lexeme::Type> operatorDict{
      {"+", Lexeme::Type::ADD},         {"-", Lexeme::Type::SUB},
      {"*", Lexeme::Type::MUL},         {"/", Lexeme::Type::DIV},
      {"rem", Lexeme::Type::REM},       {"REM", Lexeme::Type::REM},
      {"mod", Lexeme::Type::MOD},       {"MOD", Lexeme::Type::MOD},

      {">", Lexeme::Type::GT},          {"<", Lexeme::Type::LT},
      {"=", Lexeme::Type::EQ},          {"<>", Lexeme::Type::NEQ},

      {"emit", Lexeme::Type::EMIT},     {"EMIT", Lexeme::Type::EMIT},
      {".", Lexeme::Type::DOT},

      {"dup", Lexeme::Type::DUP},       {"DUP", Lexeme::Type::DUP},
      {"drop", Lexeme::Type::DROP},     {"DROP", Lexeme::Type::DROP},
      {"switch", Lexeme::Type::SWITCH}, {"SWITCH", Lexeme::Type::SWITCH},
      {"over", Lexeme::Type::OVER},     {"OVER", Lexeme::Type::OVER},
      {"rot", Lexeme::Type::ROT},       {"ROT", Lexeme::Type::ROT},

      {":", Lexeme::Type::COL},         {";", Lexeme::Type::SEMICOL},

      {"if", Lexeme::Type::IF},         {"IF", Lexeme::Type::IF},
      {"then", Lexeme::Type::THEN},     {"THEN", Lexeme::Type::THEN},

      {"begin", Lexeme::Type::BEGIN},   {"BEGIN", Lexeme::Type::BEGIN},
      {"until", Lexeme::Type::UNTIL},   {"UNTIL", Lexeme::Type::UNTIL},
  };
  if (word.empty()) {
    return {};
  } else if (auto find = operatorDict.find(word); find != operatorDict.end()) {
    return Lexeme{find->second, {}};
  } else {
    std::optional<std::int64_t> num;
    if ((num = parseInt(word))) {
      return Lexeme{Lexeme::Type::NUM, *num};
    } else {
      return Lexeme{Lexeme::Type::WORD, word};
    }
  }
}

std::optional<Lexeme> lexWord(std::FILE *const fin, std::string &word) {
  const int ch = fgetc(fin);
  if (ch == EOF || std::isspace(ch)) {
    return lexWord(word);
  } else {
    word.push_back(ch);
    return lexWord(fin, word);
  }
}

std::optional<Lexeme> lex(std::FILE *fin) {
  const int ch = fgetc(fin);
  if (ch == EOF) {
    return {};
  } else if (std::isspace(ch)) {
    return lex(fin);
  } else {
    std::string word;
    word.push_back(ch);
    return lexWord(fin, word);
  }
}

struct LexemeSource {
  enum class Type {
    NONE,
    FILE,
    COLLECTION,
  } type;
  struct Collection {
    std::vector<Lexeme> lexemes;
    size_t index;
  };
  std::variant<std::monostate, FILE *, Collection, Lexeme> data;

  LexemeSource() : type(Type::NONE), data({}) {}
  LexemeSource(FILE *const fin) : type(Type::FILE), data(fin) {}
  template <typename Iterator>
  LexemeSource(Iterator begin, Iterator end)
      : type(Type::COLLECTION), data(Collection{{begin, end}, 0}) {}

  std::optional<Lexeme> get() {
    switch (type) {
    case Type::NONE:
      return {};
      break;
    case Type::FILE:
      return lex(std::get<FILE *>(data));
      break;
    case Type::COLLECTION: {
      Collection &collection = std::get<Collection>(data);
      if (collection.index < collection.lexemes.size()) {
        const size_t lastIndex = collection.index;
        ++collection.index;
        return collection.lexemes[lastIndex];
      } else {
        return {};
      }
    } break;
    }

    fprintf(stderr, "unexpected\n");
    exit(EXIT_FAILURE);
  }
};

struct Expression {
  enum class Type {
    NUM,
    WORD,

    ADD,
    SUB,
    MUL,
    DIV,
    REM,
    MOD,

    GT,
    LT,
    EQ,

    NEQ,

    DOT,
    EMIT,

    DUP,
    DROP,
    SWITCH,
    OVER,
    ROT,

    DEF,
    IF,
    BEGIN,
  } type;
  struct Def {
    std::string word;
    std::vector<Expression> body;
  };
  std::variant<std::monostate, int64_t, std::string, Def,
               std::vector<Expression>>
      data;
};

std::optional<Expression> parse(LexemeSource &source);
std::optional<Expression> parseDefWord(LexemeSource &source);
std::optional<Expression> parseDefBody(LexemeSource &source,
                                       const std::string &word,
                                       std::vector<Lexeme> &body);
std::optional<Expression> parseIf(LexemeSource &source,
                                  std::vector<Lexeme> &body);
std::vector<Expression> parseAll(LexemeSource &source);

std::vector<Expression> parseAll(LexemeSource &source) {
  std::vector<Expression> bodyExprs;
  std::optional<Expression> expr;
  while ((expr = parse(source))) {
    bodyExprs.push_back(*expr);
  }
  return bodyExprs;
}

std::optional<Expression> parseIf(LexemeSource &source,
                                  std::vector<Lexeme> &body) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    fprintf(stderr, "unexpected EOF\n");
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::THEN: {
    LexemeSource bodySource{body.begin(), body.end()};
    return Expression{Expression::Type::IF, parseAll(bodySource)};
  } break;
  default: {
    body.push_back(*lexeme);
    return parseIf(source, body);
  } break;
  }

  fprintf(stderr, "unexpected\n");
  exit(EXIT_FAILURE);
}

std::optional<Expression> parseDefBody(LexemeSource &source,
                                       const std::string &word,
                                       std::vector<Lexeme> &body) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    fprintf(stderr, "unexpected EOF\n");
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::SEMICOL: {
    LexemeSource bodySource{body.begin(), body.end()};
    return Expression{Expression::Type::DEF,
                      Expression::Def{word, parseAll(bodySource)}};
  } break;
  default: {
    body.push_back(*lexeme);
    return parseDefBody(source, word, body);
  } break;
  }

  fprintf(stderr, "unexpected\n");
  exit(EXIT_FAILURE);
}

std::optional<Expression> parseDefWord(LexemeSource &source) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    fprintf(stderr, "unexpected EOF\n");
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::WORD: {
    const std::string &word = std::get<std::string>(lexeme->data);
    std::vector<Lexeme> body;
    return parseDefBody(source, word, body);
  } break;
  default:
    fprintf(stderr, "expected WORD\n");
    exit(EXIT_FAILURE);
    break;
  }

  fprintf(stderr, "unexpected\n");
  exit(EXIT_FAILURE);
}

std::optional<Expression> parse(LexemeSource &source) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    return {};
  }

  switch (lexeme->type) {
  case Lexeme::Type::NUM:
    return Expression{Expression::Type::NUM, std::get<int64_t>(lexeme->data)};
    break;
  case Lexeme::Type::WORD:
    return Expression{Expression::Type::WORD,
                      std::get<std::string>(lexeme->data)};
    break;
  case Lexeme::Type::ADD:
    return Expression{Expression::Type::ADD, {}};
    break;
  case Lexeme::Type::SUB:
    return Expression{Expression::Type::SUB, {}};
    break;
  case Lexeme::Type::MUL:
    return Expression{Expression::Type::MUL, {}};
    break;
  case Lexeme::Type::DIV:
    return Expression{Expression::Type::DIV, {}};
    break;
  case Lexeme::Type::REM:
    return Expression{Expression::Type::REM, {}};
    break;
  case Lexeme::Type::MOD:
    return Expression{Expression::Type::MOD, {}};
    break;
  case Lexeme::Type::GT:
    return Expression{Expression::Type::GT, {}};
    break;
  case Lexeme::Type::LT:
    return Expression{Expression::Type::LT, {}};
    break;
  case Lexeme::Type::EQ:
    return Expression{Expression::Type::EQ, {}};
    break;
  case Lexeme::Type::NEQ:
    return Expression{Expression::Type::NEQ, {}};
    break;
  case Lexeme::Type::DOT:
    return Expression{Expression::Type::DOT, {}};
    break;
  case Lexeme::Type::EMIT:
    return Expression{Expression::Type::EMIT, {}};
    break;
  case Lexeme::Type::DUP:
    return Expression{Expression::Type::DUP, {}};
    break;
  case Lexeme::Type::DROP:
    return Expression{Expression::Type::DROP, {}};
    break;
  case Lexeme::Type::SWITCH:
    return Expression{Expression::Type::SWITCH, {}};
    break;
  case Lexeme::Type::OVER:
    return Expression{Expression::Type::OVER, {}};
    break;
  case Lexeme::Type::ROT:
    return Expression{Expression::Type::ROT, {}};
    break;
  case Lexeme::Type::COL:
    return parseDefWord(source);
    break;
  case Lexeme::Type::SEMICOL:
    fprintf(stderr, "unexpected semicolon\n");
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::IF: {
    std::vector<Lexeme> lexemes;
    return parseIf(source, lexemes);
  } break;
  case Lexeme::Type::THEN:
    fprintf(stderr, "unexpected THEN\n");
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::BEGIN:
  case Lexeme::Type::UNTIL:
    fprintf(stderr, "TODO\n");
    exit(EXIT_FAILURE);
    break;
  }

  fprintf(stderr, "unexpected\n");
  exit(EXIT_FAILURE);
}

struct Engine {
  std::stack<std::int64_t> dataStack;
  std::map<std::string, std::vector<Expression>> defDict;
  void push(std::int64_t number) { dataStack.push(number); }
  std::int64_t pop() {
    if (dataStack.empty()) {
      fprintf(stderr, "empty stack\n");
      exit(EXIT_FAILURE);
    }
    const std::int64_t result = dataStack.top();
    dataStack.pop();
    return result;
  }
  void eval(const Expression &expression) {
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
      push(std::int64_t(pop() > pop()));
      break;
    case Expression::Type::LT:
      push(std::int64_t(pop() < pop()));
      break;
    case Expression::Type::EQ:
      push(std::int64_t(pop() == pop()));
      break;
    case Expression::Type::NEQ:
      push(std::int64_t(pop() != pop()));
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
    case Expression::Type::SWITCH: {
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
      if (auto find = defDict.find(word); find != defDict.end()) {
        const std::vector<Expression> &body = find->second;
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
      const bool cond = bool(pop());
      if (cond) {
        const std::vector<Expression> &body =
            std::get<std::vector<Expression>>(expression.data);
        for (const Expression &expr : body) {
          eval(expr);
        }
      }
    } break;
    case Expression::Type::BEGIN:
      fprintf(stderr, "TODO\n");
      exit(EXIT_FAILURE);
      break;
    }
  }
};

int main(int argc, char **argv) {
  Engine engine;
  bool evalStdin = (argc == 1);
  for (int i = 1; i < argc; ++i) {
    if (std::string(argv[i]) == "-") {
      evalStdin = true;
      continue;
    }
    std::FILE *const file = fopen(argv[i], "r");
    if (file == stdin) {
      evalStdin = true;
      continue;
    }
    if (!file) {
      perror(argv[i]);
      exit(EXIT_FAILURE);
    }
    LexemeSource source(file);
    std::optional<Expression> expr;
    while ((expr = parse(source))) {
      engine.eval(*expr);
    }
    fclose(file);
  }
  if (evalStdin) {
    LexemeSource source(stdin);
    std::optional<Expression> expr;
    while ((expr = parse(source))) {
      engine.eval(*expr);
    }
  }
  return EXIT_SUCCESS;
}
