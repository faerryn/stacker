#include "lexer.hh"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>

std::optional<std::int64_t> lexInt64(const std::string &ident);
std::optional<Lexeme> lexString(const std::string &word);

std::optional<Lexeme> lexWord(std::FILE *const fin, std::string &word);
int lexChar(std::FILE *const fin);
int lexEscape(std::FILE *const fin);

int lexEscape(std::FILE *const fin) {
  const int ch = fgetc(fin);

  if (ch == EOF) {
    fprintf(stderr, "unexpected EOF");
    exit(EXIT_FAILURE);
  }

  switch (ch) {
  case 'n':
    return '\n';
    break;
  case 'r':
    return '\r';
    break;
  case 't':
    return '\t';
    break;
  case 'b':
    return '\b';
    break;
  default:
    return ch;
    break;
  }
}

int lexChar(std::FILE *const fin) {
  const int ch = fgetc(fin);

  if (ch == EOF) {
    fprintf(stderr, "unexpected EOF");
    exit(EXIT_FAILURE);
  }

  int value;

  if (ch == '\\') {
    value = lexEscape(fin);
  } else {
    value = ch;
  }

  if (fgetc(fin) != '\'') {
    fprintf(stderr, "expected \'");
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

std::optional<Lexeme> lexString(const std::string &word) {
  const std::map<std::string, Lexeme::Type> operatorDict{
      {"+", Lexeme::Type::ADD},         {"-", Lexeme::Type::SUB},
      {"*", Lexeme::Type::MUL},         {"/", Lexeme::Type::DIV},
      {"rem", Lexeme::Type::REM},       {"mod", Lexeme::Type::MOD},

      {">", Lexeme::Type::GT},          {"<", Lexeme::Type::LT},
      {"=", Lexeme::Type::EQ},          {"<>", Lexeme::Type::NEQ},

      {"and", Lexeme::Type::AND},       {"or", Lexeme::Type::OR},
      {"invert", Lexeme::Type::INVERT},

      {"emit", Lexeme::Type::EMIT},     {".", Lexeme::Type::DOT},

      {"dup", Lexeme::Type::DUP},       {"drop", Lexeme::Type::DROP},
      {"swap", Lexeme::Type::SWAP},     {"over", Lexeme::Type::OVER},
      {"rot", Lexeme::Type::ROT},

      {">r", Lexeme::Type::RPUT},       {"r>", Lexeme::Type::RGET},

      {":", Lexeme::Type::COL},         {";", Lexeme::Type::SEMICOL},

      {"if", Lexeme::Type::IF},         {"then", Lexeme::Type::THEN},
      {"else", Lexeme::Type::ELSE},

      {"begin", Lexeme::Type::BEGIN},   {"until", Lexeme::Type::UNTIL},
      {"while", Lexeme::Type::WHILE},   {"repeat", Lexeme::Type::REPEAT},
      {"again", Lexeme::Type::AGAIN},
  };
  if (word.empty()) {
    return {};
  } else if (const auto &find = operatorDict.find(word);
             find != operatorDict.end()) {
    return Lexeme{find->second, {}};
  } else {
    const std::optional<std::int64_t> num = lexInt64(word);
    if (num) {
      return Lexeme{Lexeme::Type::NUM, *num};
    } else {
      return Lexeme{Lexeme::Type::WORD, word};
    }
  }
}

std::optional<Lexeme> lexWord(std::FILE *const fin, std::string &word) {
  const int ch = fgetc(fin);
  if (ch == EOF || std::isspace(ch)) {
    return lexString(word);
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
  } else if (ch == '\'') {
    return Lexeme{Lexeme::Type::NUM, lexChar(fin)};
  } else {
    std::string word;
    word.push_back(ch);
    return lexWord(fin, word);
  }
}
