#ifndef PARSER_HH
#define PARSER_HH

#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "lexer.hh"

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

Expression parseNoEOF(std::istream &source);
std::optional<Expression> parse(std::istream &is);

#endif // PARSER_HH
