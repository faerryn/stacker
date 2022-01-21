#include "lexer.hh"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <optional>

bool isDec(int ch);
std::int64_t toDec(int ch);
bool isSpace(int ch);

Lexeme lexNum(std::istream &source, std::string &word, std::int64_t sign,
              std::int64_t mag);
Lexeme lexSign(std::istream &source, std::string &word, std::int64_t sign);

char lexEscape(std::istream &source);
Lexeme lexChar(std::istream &source);
Lexeme lexStr(std::istream &source, std::string &str);

Lexeme lexWordDone(const std::string &word);
Lexeme lexWord(std::istream &source, std::string &word);

Lexeme lexChar(int ch, std::istream &source);

bool isSpace(int ch) {
  return ch == EOF || ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t';
}

bool isDec(int ch) { return '0' <= ch && ch <= '9'; }
std::int64_t toDec(int ch) { return ch - '0'; }

char lexEscape(std::istream &source) {
  const int ch = source.get();

  if (ch == EOF) {
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  if (ch == 'n') {
    return '\n';
  }
  if (ch == 'r') {
    return '\r';
  }
  if (ch == 't') {
    return '\t';
  }
  if (ch == '\b') {
    return '\b';
  }

  return char(ch);
}

Lexeme lexChar(std::istream &source) {
  const int ch = source.get();

  if (ch == EOF) {
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  char value;

  if (ch == '\\') {
    value = lexEscape(source);
  } else {
    value = char(ch);
  }

  if (source.get() != '\'') {
    std::cerr << __FILE__ << ":" << __LINE__ << ": expected single-quote\n";
    exit(EXIT_FAILURE);
  }

  return Lexeme{Lexeme::Type::Number, value};
}

Lexeme lexStr(std::istream &source, std::string &str) {
  const int ch = source.get();

  if (ch == EOF) {
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  if (ch == '\"') {
    return Lexeme{Lexeme::Type::String, str};
  }

  char value;

  if (ch == '\\') {
    value = lexEscape(source);
  } else {
    value = char(ch);
  }

  str.push_back(value);

  return lexStr(source, str);
}

Lexeme lexNum(std::istream &source, std::string &word, std::int64_t sign,
              std::int64_t mag) {
  const int ch = source.get();

  if (isSpace(ch)) {
    return Lexeme{Lexeme::Type::Number, sign * mag};
  }

  word.push_back(char(ch));

  if (isDec(ch)) {
    const std::int64_t TEN = 10;
    mag *= TEN;
    mag += toDec(ch);
    return lexNum(source, word, sign, mag);
  }

  return lexWord(source, word);
}

Lexeme lexSign(std::istream &source, std::string &word, std::int64_t sign) {
  const int ch = source.get();

  if (isSpace(ch)) {
    return lexWordDone(word);
  }

  word.push_back(char(ch));

  if (isDec(ch)) {
    return lexNum(source, word, sign, toDec(ch));
  }

  return lexWord(source, word);
}

Lexeme lexWordDone(const std::string &word) {
  const std::map<std::string, Lexeme> BUILTIN_TABLE = {

      {"+", {Lexeme::Type::Add, {}}},
      {"-", {Lexeme::Type::Sub, {}}},
      {"*", {Lexeme::Type::Mul, {}}},
      {"/", {Lexeme::Type::Div, {}}},
      {"rem", {Lexeme::Type::Rem, {}}},
      {"mod", {Lexeme::Type::Mod, {}}},

      {"<", {Lexeme::Type::Less, {}}},
      {">", {Lexeme::Type::More, {}}},
      {"=", {Lexeme::Type::Equal, {}}},
      {"<>", {Lexeme::Type::NotEqual, {}}},

      {"and", {Lexeme::Type::And, {}}},
      {"or", {Lexeme::Type::Or, {}}},
      {"invert", {Lexeme::Type::Invert, {}}},

      {"emit", {Lexeme::Type::Emit, {}}},
      {"key", {Lexeme::Type::Key, {}}},

      {"dup", {Lexeme::Type::Dup, {}}},
      {"drop", {Lexeme::Type::Drop, {}}},
      {"swap", {Lexeme::Type::Swap, {}}},
      {"over", {Lexeme::Type::Over, {}}},
      {"rot", {Lexeme::Type::Rot, {}}},

      {">r", {Lexeme::Type::ToR, {}}},
      {"r>", {Lexeme::Type::RFrom, {}}},
      {"r@", {Lexeme::Type::RFetch, {}}},

      {"!", {Lexeme::Type::Store, {}}},
      {"@", {Lexeme::Type::Fetch, {}}},
      {"c!", {Lexeme::Type::CStore, {}}},
      {"c@", {Lexeme::Type::CFetch, {}}},
      {"alloc", {Lexeme::Type::Alloc, {}}},
      {"free", {Lexeme::Type::Free, {}}},

      {".s", {Lexeme::Type::DotS, {}}},
      {"bye", {Lexeme::Type::Bye, {}}},

      {":", {Lexeme::Type::Col, {}}},
      {";", {Lexeme::Type::Semi, {}}},

      {"if", {Lexeme::Type::If, {}}},
      {"then", {Lexeme::Type::Then, {}}},
      {"else", {Lexeme::Type::Else, {}}},

      {"begin", {Lexeme::Type::Begin, {}}},
      {"until", {Lexeme::Type::Until, {}}},
      {"while", {Lexeme::Type::While, {}}},
      {"repeat", {Lexeme::Type::Repeat, {}}},
      {"again", {Lexeme::Type::Again, {}}},

  };

  const auto &find = BUILTIN_TABLE.find(word);
  if (find != BUILTIN_TABLE.end()) {
    return find->second;
  }

  return Lexeme{Lexeme::Type::Word, word};
}

Lexeme lexWord(std::istream &source, std::string &word) {
  const int ch = source.get();

  if (isSpace(ch)) {
    return lexWordDone(word);
  }

  word.push_back(char(ch));
  return lexWord(source, word);
}

std::optional<Lexeme> lex(std::istream &source) {
  const int ch = source.get();
  if (ch == EOF) {
    return {};
  }
  if (isSpace(ch)) {
    return lex(source);
  }
  return lexChar(ch, source);
}

Lexeme lexChar(int ch, std::istream &source) {
  if (ch == '\'') {
    return lexChar(source);
  }
  if (ch == '\"') {
    std::string str;
    return lexStr(source, str);
  }

  std::string word;
  word.push_back(char(ch));
  if (isDec(ch)) {
    return lexNum(source, word, +1, toDec(ch));
  }
  if (ch == '+') {
    return lexSign(source, word, +1);
  }
  if (ch == '-') {
    return lexSign(source, word, -1);
  }
  return lexWord(source, word);
}
