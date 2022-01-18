#include <cctype>
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

    GT,
    LT,
    EQ,

    NEQ,

    DOT,

    DUP,
    SWITCH,
    OVER,

    COL,
    WORD,
    SEMICOL,

    IF,
    THEN,
  } type;
  std::variant<std::monostate, std::int64_t, std::string> data;
};

std::optional<int64_t> parseInt(const std::string &ident) {
  if (ident.empty()) {
    return {};
  }
  int64_t result = 0;

  int64_t sign = +1;
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
      {"+", Lexeme::Type::ADD},     {"-", Lexeme::Type::SUB},
      {"*", Lexeme::Type::MUL},     {"/", Lexeme::Type::DIV},

      {">", Lexeme::Type::GT},      {"<", Lexeme::Type::LT},
      {"=", Lexeme::Type::EQ},      {"<>", Lexeme::Type::NEQ},

      {".", Lexeme::Type::DOT},

      {"dup", Lexeme::Type::DUP},   {"switch", Lexeme::Type::SWITCH},
      {"over", Lexeme::Type::OVER},

      {":", Lexeme::Type::COL},     {";", Lexeme::Type::SEMICOL},

      {"if", Lexeme::Type::IF},     {"then", Lexeme::Type::THEN},
  };
  if (word.empty()) {
    return {};
  } else if (auto find = operatorDict.find(word); find != operatorDict.end()) {
    return Lexeme{find->second, {}};
  } else {
    std::optional<int64_t> num;
    if ((num = parseInt(word))) {
      return Lexeme{Lexeme::Type::NUM, *num};
    } else {
      return Lexeme{Lexeme::Type::WORD, word};
    }
  }
}

std::optional<Lexeme> lexWord(FILE *fin, std::string &word) {
  const int ch = fgetc(fin);
  if (ch == EOF || std::isspace(ch)) {
    return parseWord(word);
  } else {
    word.push_back(ch);
    return lexWord(fin, word);
  }
}

std::optional<Lexeme> lex(FILE *fin) {
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
  std::stack<int64_t> dataStack;
  std::deque<Lexeme> instructionQueue;
  std::map<std::string, std::vector<Lexeme>> wordDict;
  void push(int64_t number) { dataStack.push(number); }
  int64_t pop() {
    if (dataStack.empty()) {
      fprintf(stderr, "empty stack\n");
      exit(EXIT_FAILURE);
    }
    int64_t result = dataStack.top();
    dataStack.pop();
    return result;
  }
  std::optional<Lexeme> dequeueFrom(FILE *fin) {
    if (instructionQueue.empty()) {
      return lex(fin);
    } else {
      const Lexeme result = instructionQueue.front();
      instructionQueue.pop_front();
      return result;
    }
  }
  void evalWordDefBody(FILE *fin, const std::string &word,
                       std::vector<Lexeme> &def) {
    std::optional<Lexeme> lexeme = dequeueFrom(fin);
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
  void evalWordDef(FILE *fin) {
    std::optional<Lexeme> lexeme = dequeueFrom(fin);
    if (!lexeme) {
      fprintf(stderr, "unexpected EOF\n");
      exit(EXIT_FAILURE);
    }
    switch (lexeme->type) {
    case Lexeme::Type::WORD: {
      std::string word = std::get<std::string>(lexeme->data);
      std::vector<Lexeme> def;
      evalWordDefBody(fin, word, def);
    } break;
    default:
      fprintf(stderr, "expected word\n");
      exit(EXIT_FAILURE);
      break;
    }
  }
  void evalIfSkip(FILE *fin) {
    std::optional<Lexeme> lexeme = dequeueFrom(fin);
    if (!lexeme) {
      fprintf(stderr, "unexpected EOF\n");
      exit(EXIT_FAILURE);
    }
    switch (lexeme->type) {
    case Lexeme::Type::THEN:
      break;
    default:
      evalIfSkip(fin);
      break;
    }
  }
  bool eval(FILE *fin) {
    std::optional<Lexeme> lexeme = dequeueFrom(fin);
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
    case Lexeme::Type::GT:
      push(int64_t(pop() > pop()));
      break;
    case Lexeme::Type::LT:
      push(int64_t(pop() < pop()));
      break;
    case Lexeme::Type::EQ:
      push(int64_t(pop() == pop()));
      break;
    case Lexeme::Type::NEQ:
      push(int64_t(pop() != pop()));
      break;
    case Lexeme::Type::DOT:
      printf("%ld\n", pop());
      break;
    case Lexeme::Type::DUP: {
      const int64_t top = pop();
      push(top);
      push(top);
    } break;
    case Lexeme::Type::SWITCH: {
      const int64_t b = pop();
      const int64_t a = pop();
      push(b);
      push(a);
    } break;
    case Lexeme::Type::OVER: {
      const int64_t b = pop();
      const int64_t a = pop();
      push(a);
      push(b);
      push(a);
    } break;
    case Lexeme::Type::WORD:
      if (auto find = wordDict.find(std::get<std::string>(lexeme->data));
          find != wordDict.end()) {
        const auto &def = find->second;
        instructionQueue.insert(instructionQueue.begin(), def.begin(),
                                def.end());
      } else {
        fprintf(stderr, "unknown word\n");
        exit(EXIT_FAILURE);
      }
      break;
    case Lexeme::Type::COL:
      evalWordDef(fin);
      break;
    case Lexeme::Type::SEMICOL:
      fprintf(stderr, "unexpected semicolumn\n");
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
  void evalLoop(FILE *fin) {
    bool run = true;
    while (run) {
      run = eval(fin);
    }
  }
};

int main(int argc, char **argv) {
  Engine engine;
  bool stdinEnabled = (argc == 1);
  for (int i = 1; i < argc; ++i) {
    if (std::string(argv[i]) == "-") {
      stdinEnabled = true;
    }
    FILE *const file = fopen(argv[i], "r");
    if (!file) {
      perror(argv[i]);
      exit(EXIT_FAILURE);
    }
    engine.evalLoop(file);
    fclose(file);
  }
  if (stdinEnabled) {
    engine.evalLoop(stdin);
  }
  return EXIT_SUCCESS;
}
