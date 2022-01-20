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
    Key,

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
  struct Def {
    std::string word;
    std::vector<Expression> body;
  };
  struct BeginWhile {
    std::vector<Expression> condBody;
    std::vector<Expression> whileBody;
  };
  struct IfElse {
    std::vector<Expression> ifBody;
    std::vector<Expression> elseBody;
  };
  std::variant<std::monostate, std::int64_t, std::string,
               std::vector<Expression>, Def, BeginWhile, IfElse>
      data;
};

Expression parseNoEOF(std::istream &source);
std::optional<Expression> parse(std::istream &is);

#endif // PARSER_HH
