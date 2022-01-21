#include "lexer.hh"

#include <cctype>
#include <cstdlib>
#include <optional>

bool isDec(int ch);
std::int64_t toDec(int ch);
bool isSpace(int ch);

Lexeme lexNum(std::istream &is, std::string &word, std::int64_t sign,
              std::int64_t mag);
Lexeme lexSign(std::istream &is, std::string &word, std::int64_t sign);

char lexEscape(std::istream &is);
Lexeme lexChar(std::istream &is);
Lexeme lexStr(std::istream &is, std::string &str);

Lexeme lexWordDone(const std::string &word);
Lexeme lexWord(std::istream &is, std::string &word);

bool isSpace(int ch) {
  return ch == EOF || ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t';
}

bool isDec(int ch) { return '0' <= ch && ch <= '9'; }
std::int64_t toDec(int ch) { return ch - '0'; }

char lexEscape(std::istream &is) {
  const int ch = is.get();

  if (ch == EOF) {
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected EOF\n";
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
  } else {
    return ch;
  }
}

Lexeme lexChar(std::istream &is) {
  const int ch = is.get();

  if (ch == EOF) {
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  char value;

  if (ch == '\\') {
    value = lexEscape(is);
  } else {
    value = ch;
  }

  if (is.get() != '\'') {
    std::cerr << __FILE__ << ":" << __LINE__ << ": expected single-quote\n";
    exit(EXIT_FAILURE);
  }

  return Lexeme{Lexeme::Type::Number, value};
}

Lexeme lexStr(std::istream &is, std::string &str) {
  const int ch = is.get();

  if (ch == EOF) {
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  if (ch == '\"') {
    return Lexeme{Lexeme::Type::String, str};
  }

  char value;

  if (ch == '\\') {
    value = lexEscape(is);
  } else {
    value = ch;
  }

  str.push_back(value);

  return lexStr(is, str);
}

Lexeme lexNum(std::istream &is, std::string &word, std::int64_t sign,
              std::int64_t mag) {
  const int ch = is.get();

  if (isSpace(ch)) {
    return Lexeme{Lexeme::Type::Number, sign * mag};
  }

  word.push_back(ch);

  if (isDec(ch)) {
    mag *= 10;
    mag += toDec(ch);
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

  if (isDec(ch)) {
    return lexNum(is, word, sign, toDec(ch));
  } else {
    return lexWord(is, word);
  }
}

Lexeme lexWordDone(const std::string &word) {
  if (word == "+") {
    return Lexeme{Lexeme::Type::Add, {}};
  } else if (word == "-") {
    return Lexeme{Lexeme::Type::Sub, {}};
  } else if (word == "*") {
    return Lexeme{Lexeme::Type::Mul, {}};
  } else if (word == "/") {
    return Lexeme{Lexeme::Type::Div, {}};
  } else if (word == "rem") {
    return Lexeme{Lexeme::Type::Rem, {}};
  } else if (word == "mod") {
    return Lexeme{Lexeme::Type::Mod, {}};

  } else if (word == "<") {
    return Lexeme{Lexeme::Type::Less, {}};
  } else if (word == ">") {
    return Lexeme{Lexeme::Type::More, {}};
  } else if (word == "=") {
    return Lexeme{Lexeme::Type::Equal, {}};
  } else if (word == "<>") {
    return Lexeme{Lexeme::Type::NotEqual, {}};

  } else if (word == "and") {
    return Lexeme{Lexeme::Type::And, {}};
  } else if (word == "or") {
    return Lexeme{Lexeme::Type::Or, {}};
  } else if (word == "invert") {
    return Lexeme{Lexeme::Type::Inv, {}};

  } else if (word == "emit") {
    return Lexeme{Lexeme::Type::Emit, {}};
  } else if (word == "key") {
    return Lexeme{Lexeme::Type::Key, {}};

  } else if (word == "dup") {
    return Lexeme{Lexeme::Type::Dup, {}};
  } else if (word == "drop") {
    return Lexeme{Lexeme::Type::Drop, {}};
  } else if (word == "swap") {
    return Lexeme{Lexeme::Type::Swap, {}};
  } else if (word == "over") {
    return Lexeme{Lexeme::Type::Over, {}};
  } else if (word == "rot") {
    return Lexeme{Lexeme::Type::Rot, {}};

  } else if (word == ">r") {
    return Lexeme{Lexeme::Type::ToR, {}};
  } else if (word == "r>") {
    return Lexeme{Lexeme::Type::RFrom, {}};

  } else if (word == "!") {
    return Lexeme{Lexeme::Type::Store, {}};
  } else if (word == "@") {
    return Lexeme{Lexeme::Type::Fetch, {}};
  } else if (word == "c!") {
    return Lexeme{Lexeme::Type::CStore, {}};
  } else if (word == "c@") {
    return Lexeme{Lexeme::Type::CFetch, {}};
  } else if (word == "alloc") {
    return Lexeme{Lexeme::Type::Alloc, {}};
  } else if (word == "free") {
    return Lexeme{Lexeme::Type::Free, {}};

  } else if (word == ".s") {
    return Lexeme{Lexeme::Type::DotS, {}};
  } else if (word == "bye") {
    return Lexeme{Lexeme::Type::Bye, {}};

  } else if (word == ":") {
    return Lexeme{Lexeme::Type::Col, {}};
  } else if (word == ";") {
    return Lexeme{Lexeme::Type::Semi, {}};

  } else if (word == "if") {
    return Lexeme{Lexeme::Type::If, {}};
  } else if (word == "then") {
    return Lexeme{Lexeme::Type::Then, {}};
  } else if (word == "else") {
    return Lexeme{Lexeme::Type::Else, {}};

  } else if (word == "begin") {
    return Lexeme{Lexeme::Type::Begin, {}};
  } else if (word == "until") {
    return Lexeme{Lexeme::Type::Until, {}};
  } else if (word == "while") {
    return Lexeme{Lexeme::Type::While, {}};
  } else if (word == "repeat") {
    return Lexeme{Lexeme::Type::Repeat, {}};
  } else if (word == "again") {
    return Lexeme{Lexeme::Type::Again, {}};

  } else {
    return Lexeme{Lexeme::Type::Word, word};
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
  } else if (ch == '\"') {
    std::string str;
    return lexStr(is, str);
  } else {
    std::string word;
    word.push_back(ch);
    if (isDec(ch)) {
      return lexNum(is, word, +1, toDec(ch));
    } else if (ch == '+') {
      return lexSign(is, word, +1);
    } else if (ch == '-') {
      return lexSign(is, word, -1);
    } else {
      return lexWord(is, word);
    }
  }
}

Lexeme lexNoEOF(std::istream &is) {
  std::optional<Lexeme> result = lex(is);
  if (result) {
    return *result;
  } else {
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected EOF\n";
    exit(EXIT_FAILURE);
  }
}
