#include "lexer.hh"

#include <cctype>
#include <cstdlib>
#include <optional>

std::optional<std::int64_t> chToDec(int ch);
std::optional<std::int64_t> chToHex(int ch);
std::optional<std::int64_t> chToOct(int ch);
bool isSpace(int ch);

Lexeme lexNum(std::istream &is, std::string &word, std::int64_t sign,
              std::int64_t mag);
Lexeme lexSign(std::istream &is, std::string &word, std::int64_t sign);

char lexEscape(std::istream &is);
Lexeme lexChar(std::istream &is);

Lexeme lexWordDone(const std::string &word);
Lexeme lexWord(std::istream &is, std::string &word);

bool isSpace(int ch) {
  return ch == EOF || ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t';
}

std::optional<std::int64_t> chToDec(int ch) {
  if ('0' <= ch && ch <= '9') {
    return ch - '0';
  } else {
    return {};
  }
}

std::optional<std::int64_t> chToHex(int ch) {
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

std::optional<std::int64_t> chToOct(int ch) {
  if ('0' <= ch && ch <= '7') {
    return ch - '0';
  } else {
    return {};
  }
}

char lexEscape(std::istream &is) {
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
  } else if (const std::optional<std::int64_t> &a = chToDec(ch); a) {
    const std::optional<std::int64_t> b = chToDec(is.get());
    const std::optional<std::int64_t> c = chToDec(is.get());
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
    const std::optional<std::int64_t> a = chToHex(is.get());
    const std::optional<std::int64_t> b = chToHex(is.get());
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
    const std::optional<std::int64_t> a = chToOct(is.get());
    const std::optional<std::int64_t> b = chToOct(is.get());
    const std::optional<std::int64_t> c = chToOct(is.get());
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

Lexeme lexChar(std::istream &is) {
  const int ch = is.get();

  if (ch == EOF) {
    std::cerr << "unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  char value;

  if (ch == '\\') {
    value = lexEscape(is);
  } else {
    value = ch;
  }

  if (is.get() != '\'') {
    std::cerr << "expected single-quote\n";
    exit(EXIT_FAILURE);
  }

  return Lexeme{Lexeme::Type::NUM, value};
}

Lexeme lexNum(std::istream &is, std::string &word, std::int64_t sign,
              std::int64_t mag) {
  const int ch = is.get();

  if (isSpace(ch)) {
    return Lexeme{Lexeme::Type::NUM, sign * mag};
  }

  word.push_back(ch);

  std::optional<std::int64_t> dec = chToDec(ch);
  if (dec) {
    mag *= 10;
    mag += *dec;
    return lexNum(is, word, sign, mag);
  } else {
    return lexWord(is, word);
  }
}

Lexeme lexSign(std::istream &is, std::string &word, std::int64_t sign) {
  const int ch = is.get();

  if (isSpace(ch)) {
    return lexWordDone(word);
  }

  word.push_back(ch);

  std::optional<std::int64_t> dec = chToDec(ch);
  if (dec) {
    return lexNum(is, word, sign, *dec);
  } else {
    return lexWord(is, word);
  }
}

Lexeme lexWordDone(const std::string &word) {
  if (word == "+") {
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
  } else if (word == "recurse") {
    return Lexeme{Lexeme::Type::RECURSE, {}};
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

  } else if (word == ".s") {
    return Lexeme{Lexeme::Type::DEBUG, {}};
  } else if (word == "bye") {
    return Lexeme{Lexeme::Type::BYE, {}};

  } else {
    return Lexeme{Lexeme::Type::WORD, word};
  }
}

Lexeme lexWord(std::istream &is, std::string &word) {
  const int ch = is.get();

  if (isSpace(ch)) {
    return lexWordDone(word);
  } else {
    word.push_back(ch);
    return lexWord(is, word);
  }
}

std::optional<Lexeme> lex(std::istream &is) {
  const int ch = is.get();
  if (ch == EOF) {
    return {};
  } else if (isSpace(ch)) {
    return lex(is);
  } else if (ch == '\'') {
    return lexChar(is);
  } else {
    std::string word;
    word.push_back(ch);
    std::optional<std::int64_t> dec = chToDec(ch);
    if (dec) {
      return lexNum(is, word, +1, *dec);
    } else if (ch == '+') {
      return lexSign(is, word, +1);
    } else if (ch == '-') {
      return lexSign(is, word, -1);
    } else {
      return lexWord(is, word);
    }
  }
}
