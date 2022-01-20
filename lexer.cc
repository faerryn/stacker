#include "lexer.hh"

#include <cctype>
#include <cstdlib>
#include <map>

std::optional<std::int64_t> lexInt64(const std::string &ident);
std::optional<Lexeme> lexString(const std::string &word);

std::optional<Lexeme> lexWord(std::istream &is, std::string &word);
int lexChar(std::istream &is);
int lexEscape(std::istream &is);

std::optional<int> chToDec(int ch) {
  if ('0' <= ch && ch <= '9') {
    return ch - '0';
  } else {
    return {};
  }
}

std::optional<int> chToHex(int ch) {
  if ('0' <= ch && ch <= '9') {
    return ch - '0';
  } else if ('A' <= ch && ch <= 'F') {
    return 10 + ch - 'A';
  } else if ('a' <= ch && ch <= 'f') {
    return 10 + ch - 'a';
  } else {
    return {};
  }
}

std::optional<int> chToOct(int ch) {
  if ('0' <= ch && ch <= '7') {
    return ch - '0';
  } else {
    return {};
  }
}

int lexEscape(std::istream &is) {
  const int ch = is.get();

  if (ch == EOF) {
    std::cerr << "unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  if (ch == 'n') {
    return '\n';
  } else if (ch == 'r') {
    return '\r';
  } else if (ch == 't') {
    return '\t';
  } else if (ch == '\b') {
    return '\b';
  } else if (const std::optional<int> &a = chToDec(ch); a) {
    const std::optional<int> b = chToDec(is.get());
    const std::optional<int> c = chToDec(is.get());
    if (!b || !c) {
      std::cerr << "expected decimal\n";
      exit(EXIT_FAILURE);
    }
    const int value = 100 * *a + 10 * *b + *c;
    if (value > 255) {
      std::cerr << "character out-of-bounds\n";
      exit(EXIT_FAILURE);
    }
    return value;
  } else if (ch == 'x') {
    const std::optional<int> a = chToHex(is.get());
    const std::optional<int> b = chToHex(is.get());
    if (!a || !b) {
      std::cerr << "expected hexadecimal\n";
      exit(EXIT_FAILURE);
    }
    const int value = 16 * *a + *b;
    if (value > 255) {
      std::cerr << "character out-of-bounds\n";
      exit(EXIT_FAILURE);
    }
    return value;
  } else if (ch == 'o') {
    const std::optional<int> a = chToOct(is.get());
    const std::optional<int> b = chToOct(is.get());
    const std::optional<int> c = chToOct(is.get());
    if (!a || !b || !c) {
      std::cerr << "expected octal\n";
      exit(EXIT_FAILURE);
    }
    const int value = 64 * *a + 8 * *b + *c;
    if (value > 255) {
      std::cerr << "character out-of-bounds\n";
      exit(EXIT_FAILURE);
    }
    return value;
  } else {
    return ch;
  }
}

int lexChar(std::istream &is) {
  const int ch = is.get();

  if (ch == EOF) {
    std::cerr << "unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  int value;

  if (ch == '\\') {
    value = lexEscape(is);
  } else {
    value = ch;
  }

  if (is.get() != '\'') {
    std::cerr << "expected single-quote\n";
    exit(EXIT_FAILURE);
  }

  return value;
}

std::optional<std::int64_t> lexInt64(const std::string &ident) {
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
    const std::optional<int> dec = chToDec(ident[i]);
    if (dec) {
      result *= 10;
      result += *dec;
    } else {
      return {};
    }
    ++i;
  }

  return sign * result;
}

std::optional<Lexeme> lexString(const std::string &word) {
  if (word.empty()) {
    return {};

  } else if (word == "+") {
    return Lexeme{Lexeme::Type::ADD, {}};
  } else if (word == "-") {
    return Lexeme{Lexeme::Type::SUB, {}};
  } else if (word == "*") {
    return Lexeme{Lexeme::Type::MUL, {}};
  } else if (word == "/") {
    return Lexeme{Lexeme::Type::DIV, {}};
  } else if (word == "rem") {
    return Lexeme{Lexeme::Type::REM, {}};
  } else if (word == "mod") {
    return Lexeme{Lexeme::Type::MOD, {}};

  } else if (word == ">") {
    return Lexeme{Lexeme::Type::GT, {}};
  } else if (word == "<") {
    return Lexeme{Lexeme::Type::LT, {}};
  } else if (word == "=") {
    return Lexeme{Lexeme::Type::EQ, {}};
  } else if (word == "<>") {
    return Lexeme{Lexeme::Type::NEQ, {}};

  } else if (word == "and") {
    return Lexeme{Lexeme::Type::AND, {}};
  } else if (word == "or") {
    return Lexeme{Lexeme::Type::OR, {}};
  } else if (word == "invert") {
    return Lexeme{Lexeme::Type::INVERT, {}};

  } else if (word == "emit") {
    return Lexeme{Lexeme::Type::EMIT, {}};
  } else if (word == ".") {
    return Lexeme{Lexeme::Type::DOT, {}};

  } else if (word == "dup") {
    return Lexeme{Lexeme::Type::DUP, {}};
  } else if (word == "drop") {
    return Lexeme{Lexeme::Type::DROP, {}};
  } else if (word == "swap") {
    return Lexeme{Lexeme::Type::SWAP, {}};
  } else if (word == "over") {
    return Lexeme{Lexeme::Type::OVER, {}};
  } else if (word == "rot") {
    return Lexeme{Lexeme::Type::ROT, {}};

  } else if (word == ">r") {
    return Lexeme{Lexeme::Type::RPUT, {}};
  } else if (word == "r>") {
    return Lexeme{Lexeme::Type::RGET, {}};

  } else if (word == ":") {
    return Lexeme{Lexeme::Type::COL, {}};
  } else if (word == ";") {
    return Lexeme{Lexeme::Type::SEMICOL, {}};

  } else if (word == "if") {
    return Lexeme{Lexeme::Type::IF, {}};
  } else if (word == "then") {
    return Lexeme{Lexeme::Type::THEN, {}};
  } else if (word == "else") {
    return Lexeme{Lexeme::Type::ELSE, {}};

  } else if (word == "begin") {
    return Lexeme{Lexeme::Type::BEGIN, {}};
  } else if (word == "until") {
    return Lexeme{Lexeme::Type::UNTIL, {}};
  } else if (word == "while") {
    return Lexeme{Lexeme::Type::WHILE, {}};
  } else if (word == "repeat") {
    return Lexeme{Lexeme::Type::REPEAT, {}};
  } else if (word == "again") {
    return Lexeme{Lexeme::Type::AGAIN, {}};

  } else if (word == "variable") {
    return Lexeme{Lexeme::Type::VARIABLE, {}};
  } else if (word == "!") {
    return Lexeme{Lexeme::Type::STORE, {}};
  } else if (word == "@") {
    return Lexeme{Lexeme::Type::FETCH, {}};

  } else {
    const std::optional<std::int64_t> num = lexInt64(word);
    if (num) {
      return Lexeme{Lexeme::Type::NUM, *num};
    } else {
      return Lexeme{Lexeme::Type::WORD, word};
    }
  }
}

std::optional<Lexeme> lexWord(std::istream &is, std::string &word) {
  const int ch = is.get();
  if (ch == EOF || std::isspace(ch)) {
    return lexString(word);
  } else {
    word.push_back(ch);
    return lexWord(is, word);
  }
}

std::optional<Lexeme> lex(std::istream &is) {
  const int ch = is.get();
  if (ch == EOF) {
    return {};
  } else if (std::isspace(ch)) {
    return lex(is);
  } else if (ch == '\'') {
    return Lexeme{Lexeme::Type::NUM, lexChar(is)};
  } else {
    std::string word;
    word.push_back(ch);
    return lexWord(is, word);
  }
}
