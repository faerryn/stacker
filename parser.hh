#ifndef PARSER_HH
#define PARSER_HH

#include <cstdint>
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

    AND,
    OR,
    INVERT,

    DOT,
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
