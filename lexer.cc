#include "lexer.hh"

#include <cstdio>
#include <cstdlib>
#include <map>

std::optional<std::int64_t> lexInt(const std::string &ident) {
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
      {"+", Lexeme::Type::ADD},       {"-", Lexeme::Type::SUB},
      {"*", Lexeme::Type::MUL},       {"/", Lexeme::Type::DIV},
      {"rem", Lexeme::Type::REM},     {"mod", Lexeme::Type::MOD},

      {">", Lexeme::Type::GT},        {"<", Lexeme::Type::LT},
      {"=", Lexeme::Type::EQ},        {"<>", Lexeme::Type::NEQ},

      {"emit", Lexeme::Type::EMIT},   {".", Lexeme::Type::DOT},

      {"dup", Lexeme::Type::DUP},     {"drop", Lexeme::Type::DROP},
      {"swap", Lexeme::Type::SWAP},   {"over", Lexeme::Type::OVER},
      {"rot", Lexeme::Type::ROT},

      {":", Lexeme::Type::COL},       {";", Lexeme::Type::SEMICOL},

      {"if", Lexeme::Type::IF},       {"then", Lexeme::Type::THEN},

      {"begin", Lexeme::Type::BEGIN}, {"until", Lexeme::Type::UNTIL},
      {"while", Lexeme::Type::WHILE}, {"repeat", Lexeme::Type::REPEAT},
      {"again", Lexeme::Type::AGAIN},
  };
  if (word.empty()) {
    return {};
  } else if (auto find = operatorDict.find(word); find != operatorDict.end()) {
    return Lexeme{find->second, {}};
  } else {
    std::optional<std::int64_t> num;
    if ((num = lexInt(word))) {
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
