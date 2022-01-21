#include "parser.hh"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>

#include "lexer.hh"

Expression parseDefineWord(std::istream &source);
Expression parseDefineBody(std::istream &source, const std::string &word,
                           std::vector<Expression> &body);
Expression parseIf(std::istream &source, std::vector<Expression> &body);
Expression parseIfElse(std::istream &source,
                       const std::vector<Expression> &ifBody,
                       std::vector<Expression> &body);
Expression parseBegin(std::istream &source, std::vector<Expression> &body);
Expression parseBeginWhile(std::istream &source,
                           const std::vector<Expression> &cond,
                           std::vector<Expression> &body);
Expression parseVariable(std::istream &source);
std::vector<Expression> parseAll(std::istream &source);
Expression parseLexeme(const Lexeme &lexeme, std::istream &source);
Lexeme lexNoEOF(std::istream &is);

Lexeme lexNoEOF(std::istream &is) {
  std::optional<Lexeme> result = lex(is);
  if (result) {
    return *result;
  } else {
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected EOF\n";
    exit(EXIT_FAILURE);
  }
}

std::vector<Expression> parseAll(std::istream &source) {
  std::vector<Expression> body;
  std::optional<Expression> expr;
  while ((expr = parse(source))) {
    body.push_back(*expr);
  }
  return body;
}

Expression parseBeginWhile(std::istream &source,
                           const std::vector<Expression> &cond,
                           std::vector<Expression> &body) {
  const Lexeme lexeme = lexNoEOF(source);

  switch (lexeme.type) {
  case Lexeme::Type::Repeat: {
    return Expression{Expression::Type::BeginWhileRepeat,
                      Expression::BeginWhile{cond, body}};
  } break;
  default: {
    body.push_back(parseLexeme(lexeme, source));
    return parseBeginWhile(source, cond, body);

  } break;
  }

  std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected\n";
  exit(EXIT_FAILURE);
}

Expression parseBegin(std::istream &source, std::vector<Expression> &body) {
  const Lexeme lexeme = lexNoEOF(source);

  switch (lexeme.type) {
  case Lexeme::Type::Until: {
    return Expression{Expression::Type::BeginUntil, body};
  } break;
  case Lexeme::Type::While: {
    std::vector<Expression> whileBody;
    return parseBeginWhile(source, body, whileBody);
  } break;
  case Lexeme::Type::Again: {
    return Expression{Expression::Type::BeginAgain, body};
  } break;
  default:
    body.push_back(parseLexeme(lexeme, source));
    return parseBegin(source, body);
    break;
  }

  std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected\n";
  exit(EXIT_FAILURE);
}

Expression parseIfElse(std::istream &source,
                       const std::vector<Expression> &ifBody,
                       std::vector<Expression> &body) {
  const Lexeme lexeme = lexNoEOF(source);

  switch (lexeme.type) {
  case Lexeme::Type::Then: {
    return Expression{Expression::Type::IfElseThen,
                      Expression::IfElse{ifBody, body}};
  } break;
  default: {
    body.push_back(parseLexeme(lexeme, source));
    return parseIfElse(source, ifBody, body);
  } break;
  }

  std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected\n";
  exit(EXIT_FAILURE);
}

Expression parseIf(std::istream &source, std::vector<Expression> &body) {
  const Lexeme lexeme = lexNoEOF(source);

  switch (lexeme.type) {
  case Lexeme::Type::Then: {
    return Expression{Expression::Type::IfThen, body};
  } break;
  case Lexeme::Type::Else: {
    std::vector<Expression> elseBody;
    return parseIfElse(source, body, elseBody);
  }
  default: {
    body.push_back(parseLexeme(lexeme, source));
    return parseIf(source, body);
  } break;
  }

  std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected\n";
  exit(EXIT_FAILURE);
}

Expression parseDefineBody(std::istream &source, const std::string &word,
                           std::vector<Expression> &body) {
  const Lexeme lexeme = lexNoEOF(source);

  switch (lexeme.type) {
  case Lexeme::Type::Semi: {
    return Expression{Expression::Type::Define, Expression::Def{word, body}};
  } break;
  case Lexeme::Type::Col:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected col\n";
    exit(EXIT_FAILURE);
    break;
  default: {
    body.push_back(parseLexeme(lexeme, source));
    return parseDefineBody(source, word, body);
  } break;
  }

  std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected\n";
  exit(EXIT_FAILURE);
}

Expression parseDefineWord(std::istream &source) {
  const Lexeme lexeme = lexNoEOF(source);

  switch (lexeme.type) {
  case Lexeme::Type::Word: {
    const std::string &word = std::get<std::string>(lexeme.data);
    std::vector<Expression> body;
    return parseDefineBody(source, word, body);
  } break;
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": expected WORD\n";
    exit(EXIT_FAILURE);
    break;
  }

  std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected\n";
  exit(EXIT_FAILURE);
}

std::optional<Expression> parse(std::istream &source) {
  std::optional<Lexeme> lexeme = lex(source);
  if (lexeme) {
    return parseLexeme(*lexeme, source);
  } else {
    return {};
  }
}

Expression parseLexeme(const Lexeme &lexeme, std::istream &source) {
  switch (lexeme.type) {

  case Lexeme::Type::Number:
    return Expression{Expression::Type::Number,
                      std::get<std::int64_t>(lexeme.data)};
    break;
  case Lexeme::Type::String:
    return Expression{Expression::Type::String,
                      std::get<std::string>(lexeme.data)};
    break;
  case Lexeme::Type::Word:
    return Expression{Expression::Type::Word,
                      std::get<std::string>(lexeme.data)};
    break;

  case Lexeme::Type::Add:
    return Expression{Expression::Type::Add, {}};
    break;
  case Lexeme::Type::Sub:
    return Expression{Expression::Type::Sub, {}};
    break;
  case Lexeme::Type::Mul:
    return Expression{Expression::Type::Mul, {}};
    break;
  case Lexeme::Type::Div:
    return Expression{Expression::Type::Div, {}};
    break;
  case Lexeme::Type::Rem:
    return Expression{Expression::Type::Rem, {}};
    break;
  case Lexeme::Type::Mod:
    return Expression{Expression::Type::Mod, {}};
    break;

  case Lexeme::Type::Less:
    return Expression{Expression::Type::Less, {}};
    break;
  case Lexeme::Type::More:
    return Expression{Expression::Type::More, {}};
    break;
  case Lexeme::Type::Equal:
    return Expression{Expression::Type::Equal, {}};
    break;
  case Lexeme::Type::NotEqual:
    return Expression{Expression::Type::NotEqual, {}};
    break;

  case Lexeme::Type::And:
    return Expression{Expression::Type::And, {}};
    break;
  case Lexeme::Type::Or:
    return Expression{Expression::Type::Or, {}};
    break;
  case Lexeme::Type::Inv:
    return Expression{Expression::Type::Inv, {}};
    break;

  case Lexeme::Type::Emit:
    return Expression{Expression::Type::Emit, {}};
    break;
  case Lexeme::Type::Key:
    return Expression{Expression::Type::Key, {}};
    break;

  case Lexeme::Type::Dup:
    return Expression{Expression::Type::Dup, {}};
    break;
  case Lexeme::Type::Drop:
    return Expression{Expression::Type::Drop, {}};
    break;
  case Lexeme::Type::Swap:
    return Expression{Expression::Type::Swap, {}};
    break;
  case Lexeme::Type::Over:
    return Expression{Expression::Type::Over, {}};
    break;
  case Lexeme::Type::Rot:
    return Expression{Expression::Type::Rot, {}};
    break;

  case Lexeme::Type::ToR:
    return Expression{Expression::Type::ToR, {}};
    break;
  case Lexeme::Type::RFrom:
    return Expression{Expression::Type::RFrom, {}};
    break;

  case Lexeme::Type::Store:
    return Expression{Expression::Type::Store, {}};
    break;
  case Lexeme::Type::Fetch:
    return Expression{Expression::Type::Fetch, {}};
    break;
  case Lexeme::Type::CStore:
    return Expression{Expression::Type::CStore, {}};
    break;
  case Lexeme::Type::CFetch:
    return Expression{Expression::Type::CFetch, {}};
    break;
  case Lexeme::Type::Alloc:
    return Expression{Expression::Type::Alloc, {}};
    break;
  case Lexeme::Type::Free:
    return Expression{Expression::Type::Free, {}};
    break;

  case Lexeme::Type::DotS:
    return Expression{Expression::Type::DotS, {}};
    break;
  case Lexeme::Type::Bye:
    return Expression{Expression::Type::Bye, {}};
    break;

  case Lexeme::Type::Col:
    return parseDefineWord(source);
    break;
  case Lexeme::Type::Semi:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected semicolon\n";
    exit(EXIT_FAILURE);
    break;

  case Lexeme::Type::If: {
    std::vector<Expression> lexemes;
    return parseIf(source, lexemes);
  } break;
  case Lexeme::Type::Then:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected THEN\n";
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::Else:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected ELSE\n";
    exit(EXIT_FAILURE);
    break;

  case Lexeme::Type::Begin: {
    std::vector<Expression> lexemes;
    return parseBegin(source, lexemes);
  } break;
  case Lexeme::Type::Until:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected UNTIL\n";
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::While:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected WHILE\n";
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::Repeat:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected REPEAT\n";
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::Again:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected AGAIN\n";
    exit(EXIT_FAILURE);
    break;
  }

  std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected\n";
  exit(EXIT_FAILURE);
}
