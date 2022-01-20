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

    Plus,
    Minus,
    Times,
    Div,
    REM,
    MOD,

    less,
    more,
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

    ToR,
    RFrom,

    Colon,
    Recurse,
    Semi,

    IF,
    THEN,
    ELSE,

    BEGIN,
    UNTIL,
    WHILE,
    REPEAT,
    AGAIN,

    VARIABLE,
    Store,
    Fetch,
    CStore,
    CFetch,

    DotS,
    BYE,
  } type;
  std::variant<std::monostate, std::int64_t, std::string> data;
};

std::optional<Lexeme> lex(std::istream &is);

#endif // LEXER_HH
