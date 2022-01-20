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
    Word,
    String,

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

    Dup,
    Drop,
    Swap,
    Over,
    Rot,

    ToR,
    RFrom,

    Col,
    Rec,
    Semi,

    If,
    Then,
    Else,

    Begin,
    Until,
    While,
    Repeat,
    Again,

    Store,
    Fetch,
    CStore,
    CFetch,
    Alloc,
    Free,

    DotS,
    Bye,
  } type;
  std::variant<std::monostate, std::int64_t, std::string> data;
};

std::optional<Lexeme> lex(std::istream &is);

#endif // LEXER_HH
