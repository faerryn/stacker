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
    Stream,
    Collection,
  } type;
  struct Collection {
    std::vector<Lexeme> lexemes;
    size_t index;
  };
  std::variant<std::istream *, Collection> data;

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

    Add,
    Sub,
    Mul,
    Div,
    Rem,
    Mod,

    More,
    Less,
    Equal,
    NotEqual,

    And,
    Or,
    Inv,

    Emit,

    Dup,
    Drop,
    Swap,
    Over,
    Rot,

    ToR,
    RFrom,

    Define,

    IfThen,
    IfElseThen,

    BeginUntil,
    BeginWhileRepeat,
    BeginAgain,

    Store,
    Fetch,
    CStore,
    CFetch,
    Alloc,
    Free,

    DotS,
    Bye,
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
