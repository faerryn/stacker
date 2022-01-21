#ifndef LEXER_HH
#define LEXER_HH

#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <variant>

struct Lexeme {
  enum class Type {
    Number,
    String,
    Word,

    Add,
    Sub,
    Mul,
    Div,
    Rem,
    Mod,

    Less,
    More,
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
    RFetch,

    Store,
    Fetch,
    CStore,
    CFetch,
    Alloc,
    Free,

    DotS,
    Bye,

    Col,
    Semi,

    If,
    Then,
    Else,

    Begin,
    Until,
    While,
    Repeat,
    Again,
  } type;
  std::variant<std::monostate, std::int64_t, std::string> data;
};

std::optional<Lexeme> lex(std::istream &is);

#endif // LEXER_HH
