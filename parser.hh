#ifndef PARSER_HH
#define PARSER_HH

#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
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
  std::variant<std::monostate, std::istream *, Collection> data;

public:
  LexemeSource() = delete;
  LexemeSource(std::istream *is);
  LexemeSource(const std::vector<Lexeme> &lexemes);

  std::optional<Lexeme> get();
};

struct Expression {
  enum class Type {
    Number,
    Word,
    String,

    Plus,
    Minus,
    Times,
    Div,
    REM,
    MOD,

    more,
    less,
    Equal,
    ne,

    AND,
    OR,
    INVERT,

    d,
    EMIT,

    DUP,
    DROP,
    SWAP,
    OVER,
    ROT,

    RPUT,
    RGET,

    DEF,

    IF,
    IF_ELSE,

    BEGIN,
    BEGIN_WHILE,
    BEGIN_AGAIN,

    VARIABLE,
    STORE,
    FETCH,
    CSTORE,
    CFETCH,

    DEBUG,
    BYE,
  } type;
  using Body = std::vector<Expression>;
  struct Def {
    std::string word;
    Body body;
  };
  struct BeginWhile {
    Body condBody;
    Body whileBody;
  };
  struct IfElse {
    Body ifBody;
    Body elseBody;
  };
  std::variant<std::monostate, std::int64_t, std::string, Body, Def, BeginWhile,
               IfElse>
      data;
};

std::optional<Expression> parse(LexemeSource &source);

#endif // PARSER_HH
