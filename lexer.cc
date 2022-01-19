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

int lexEscape(std::FILE *const fin) {
  const int ch = fgetc(fin);

  if (ch == EOF) {
    fprintf(stderr, "unexpected EOF\n");
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
    const std::optional<int> b = chToDec(fgetc(fin));
    const std::optional<int> c = chToDec(fgetc(fin));
    if (!b || !c) {
      fprintf(stderr, "expected decimal\n");
      exit(EXIT_FAILURE);
    }
    const int value = 100 * *a + 10 * *b + *c;
    if (value > 255) {
      fprintf(stderr, "character out-of-bounds\n");
      exit(EXIT_FAILURE);
    }
    return value;
  } else if (ch == 'x') {
    const std::optional<int> a = chToHex(fgetc(fin));
    const std::optional<int> b = chToHex(fgetc(fin));
    if (!a || !b) {
      fprintf(stderr, "expected hexadecimal\n");
      exit(EXIT_FAILURE);
    }
    const int value = 16 * *a + *b;
    if (value > 255) {
      fprintf(stderr, "character out-of-bounds\n");
      exit(EXIT_FAILURE);
    }
    return value;
  } else if (ch == 'o') {
    const std::optional<int> a = chToOct(fgetc(fin));
    const std::optional<int> b = chToOct(fgetc(fin));
    const std::optional<int> c = chToOct(fgetc(fin));
    if (!a || !b || !c) {
      fprintf(stderr, "expected octal\n");
      exit(EXIT_FAILURE);
    }
    const int value = 64 * *a + 8 * *b + *c;
    if (value > 255) {
      fprintf(stderr, "character out-of-bounds\n");
      exit(EXIT_FAILURE);
    }
    return value;
  } else {
    return ch;
  }
}

int lexChar(std::FILE *const fin) {
  const int ch = fgetc(fin);

  if (ch == EOF) {
    fprintf(stderr, "unexpected EOF\n");
    exit(EXIT_FAILURE);
  }

  int value;

  if (ch == '\\') {
    value = lexEscape(fin);
  } else {
    value = ch;
  }

  if (fgetc(fin) != '\'') {
    fprintf(stderr, "expected single-quote\n");
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
  const auto &find = operatorDict.find(word);
  if (find != operatorDict.end()) {
    return Lexeme{find->second, {}};
  } else if (word.empty()) {
    return {};
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
