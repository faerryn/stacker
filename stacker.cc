#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <map>
#include <optional>
#include <stack>
#include <string>
#include <variant>
#include <vector>

struct Lexeme {
  enum class Type {
    NUM,

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
    WORD,
    SEMICOL,

    IF,
    THEN,
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

std::optional<Lexeme> parseWord(const std::string &word) {
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
    return parseWord(word);
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

struct Engine {
  std::stack<std::int64_t> dataStack;
  std::deque<Lexeme> instructionQueue;
  std::map<std::string, std::vector<Lexeme>> wordDict;
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
  std::optional<Lexeme> dequeueFrom(std::FILE *const fin) {
    if (instructionQueue.empty()) {
      return lex(fin);
    } else {
      const Lexeme result = instructionQueue.front();
      instructionQueue.pop_front();
      return result;
    }
  }
  void evalWordDefBody(std::FILE *const fin, const std::string &word,
                       std::vector<Lexeme> &def) {
    const std::optional<Lexeme> lexeme = dequeueFrom(fin);
    if (!lexeme) {
      fprintf(stderr, "unexpected EOF\n");
      exit(EXIT_FAILURE);
    }
    switch (lexeme->type) {
    case Lexeme::Type::SEMICOL:
      wordDict[word] = def;
      break;
    case Lexeme::Type::COL:
      fprintf(stderr, "unexpected colon\n");
      exit(EXIT_FAILURE);
      break;
    default:
      def.push_back(*lexeme);
      evalWordDefBody(fin, word, def);
      break;
    }
  }
  void evalWordDef(std::FILE *const fin) {
    const std::optional<Lexeme> lexeme = dequeueFrom(fin);
    if (!lexeme) {
      fprintf(stderr, "unexpected EOF\n");
      exit(EXIT_FAILURE);
    }
    switch (lexeme->type) {
    case Lexeme::Type::WORD: {
      const std::string &word = std::get<std::string>(lexeme->data);
      std::vector<Lexeme> def;
      evalWordDefBody(fin, word, def);
    } break;
    default:
      fprintf(stderr, "expected word\n");
      exit(EXIT_FAILURE);
      break;
    }
  }
  void evalIfSkip(std::FILE *const fin) {
    const std::optional<Lexeme> lexeme = dequeueFrom(fin);
    if (!lexeme) {
      fprintf(stderr, "unexpected EOF\n");
      exit(EXIT_FAILURE);
    }
    switch (lexeme->type) {
    case Lexeme::Type::IF:
      evalIfSkip(fin);
      evalIfSkip(fin);
      break;
    case Lexeme::Type::THEN:
      break;
    default:
      evalIfSkip(fin);
      break;
    }
  }
  bool eval(std::FILE *const fin) {
    const std::optional<Lexeme> lexeme = dequeueFrom(fin);
    if (!lexeme) {
      return false;
    }
    switch (lexeme->type) {
    case Lexeme::Type::NUM:
      push(std::get<std::int64_t>(lexeme->data));
      break;
    case Lexeme::Type::ADD:
      push(pop() + pop());
      break;
    case Lexeme::Type::SUB:
      push(pop() - pop());
      break;
    case Lexeme::Type::MUL:
      push(pop() * pop());
      break;
    case Lexeme::Type::DIV:
      push(pop() / pop());
      break;
    case Lexeme::Type::REM:
      push(pop() % pop());
      break;
    case Lexeme::Type::MOD: {
      const std::int64_t dividend = pop();
      const std::int64_t divisor = pop();
      const std::int64_t modulus = (dividend % divisor + divisor) % divisor;
      push(modulus);
    } break;
    case Lexeme::Type::GT:
      push(std::int64_t(pop() > pop()));
      break;
    case Lexeme::Type::LT:
      push(std::int64_t(pop() < pop()));
      break;
    case Lexeme::Type::EQ:
      push(std::int64_t(pop() == pop()));
      break;
    case Lexeme::Type::NEQ:
      push(std::int64_t(pop() != pop()));
      break;
    case Lexeme::Type::DOT:
      printf("%ld ", pop());
      break;
    case Lexeme::Type::EMIT:
      printf("%c", char(pop()));
      break;
    case Lexeme::Type::DUP: {
      const std::int64_t top = pop();
      push(top);
      push(top);
    } break;
    case Lexeme::Type::DROP:
      pop();
      break;
    case Lexeme::Type::SWITCH: {
      const std::int64_t b = pop();
      const std::int64_t a = pop();
      push(b);
      push(a);
    } break;
    case Lexeme::Type::OVER: {
      const std::int64_t b = pop();
      const std::int64_t a = pop();
      push(a);
      push(b);
      push(a);
    } break;
    case Lexeme::Type::ROT: {
      const std::int64_t c = pop();
      const std::int64_t b = pop();
      const std::int64_t a = pop();
      push(b);
      push(c);
      push(a);
    } break;
    case Lexeme::Type::WORD: {
      const std::string &word = std::get<std::string>(lexeme->data);
      if (auto find = wordDict.find(word); find != wordDict.end()) {
        const auto &def = find->second;
        instructionQueue.insert(instructionQueue.begin(), def.begin(),
                                def.end());
      } else {
        fprintf(stderr, "unknown word\n");
        exit(EXIT_FAILURE);
      }
    } break;
    case Lexeme::Type::COL:
      evalWordDef(fin);
      break;
    case Lexeme::Type::SEMICOL:
      fprintf(stderr, "unexpected semicolon\n");
      exit(EXIT_FAILURE);
      break;
    case Lexeme::Type::IF:
      if (!bool(pop())) {
        evalIfSkip(fin);
      }
      break;
    case Lexeme::Type::THEN:
      break;
    }
    return true;
  }
  void evalLoop(std::FILE *const fin) {
    bool run = true;
    while (run) {
      run = eval(fin);
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
    engine.evalLoop(file);
    fclose(file);
  }
  if (evalStdin) {
    engine.evalLoop(stdin);
  }
  return EXIT_SUCCESS;
}
