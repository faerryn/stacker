#ifndef PARSER_HH
#define PARSER_HH

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <optional>
#include <variant>
#include <vector>

#include "lexer.hh"

class LexemeSource {
private:
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
public:
  LexemeSource() : type(Type::NONE), data({}) {}
  LexemeSource(FILE *const fin) : type(Type::FILE), data(fin) {}
  template <typename Iterator>
  LexemeSource(Iterator begin, Iterator end)
    : type(Type::COLLECTION), data(Collection{{begin, end}, 0}) {}

  std::optional<Lexeme> get();
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
std::optional<Expression> parseBegin(LexemeSource &source,
                                     std::vector<Lexeme> &body);
std::vector<Expression> parseAll(LexemeSource &source);

#endif // PARSER_HH
