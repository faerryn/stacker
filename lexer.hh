#ifndef LEXER_HH
#define LEXER_HH

#include <cstdint>
#include <optional>
#include <string>
#include <variant>

struct Lexeme {
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

    COL,
    SEMICOL,

    IF,
    THEN,
    ELSE,

    BEGIN,
    UNTIL,
    WHILE,
    REPEAT,
    AGAIN,
  } type;
  std::variant<std::monostate, std::int64_t, std::string> data;
};

std::optional<Lexeme> lex(std::FILE *fin);

#endif // LEXER_HH
