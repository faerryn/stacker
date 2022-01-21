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
Lexeme lexNoEOF(std::istream &source);

Lexeme lexNoEOF(std::istream &source) {
  std::optional<Lexeme> result = lex(source);
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
  case Lexeme::Type::String:
    return Expression{Expression::Type::String,
                      std::get<std::string>(lexeme.data)};
  case Lexeme::Type::Word:
    return Expression{Expression::Type::Word,
                      std::get<std::string>(lexeme.data)};

  case Lexeme::Type::Add:
    return Expression{Expression::Type::Add, {}};
  case Lexeme::Type::Sub:
    return Expression{Expression::Type::Sub, {}};
  case Lexeme::Type::Mul:
    return Expression{Expression::Type::Mul, {}};
  case Lexeme::Type::Div:
    return Expression{Expression::Type::Div, {}};
  case Lexeme::Type::Rem:
    return Expression{Expression::Type::Rem, {}};
  case Lexeme::Type::Mod:
    return Expression{Expression::Type::Mod, {}};

  case Lexeme::Type::Less:
    return Expression{Expression::Type::Less, {}};
  case Lexeme::Type::More:
    return Expression{Expression::Type::More, {}};
  case Lexeme::Type::Equal:
    return Expression{Expression::Type::Equal, {}};
  case Lexeme::Type::NotEqual:
    return Expression{Expression::Type::NotEqual, {}};

  case Lexeme::Type::And:
    return Expression{Expression::Type::And, {}};
  case Lexeme::Type::Or:
    return Expression{Expression::Type::Or, {}};
  case Lexeme::Type::Inv:
    return Expression{Expression::Type::Inv, {}};

  case Lexeme::Type::Emit:
    return Expression{Expression::Type::Emit, {}};
  case Lexeme::Type::Key:
    return Expression{Expression::Type::Key, {}};

  case Lexeme::Type::Dup:
    return Expression{Expression::Type::Dup, {}};
  case Lexeme::Type::Drop:
    return Expression{Expression::Type::Drop, {}};
  case Lexeme::Type::Swap:
    return Expression{Expression::Type::Swap, {}};
  case Lexeme::Type::Over:
    return Expression{Expression::Type::Over, {}};
  case Lexeme::Type::Rot:
    return Expression{Expression::Type::Rot, {}};

  case Lexeme::Type::ToR:
    return Expression{Expression::Type::ToR, {}};
  case Lexeme::Type::RFrom:
    return Expression{Expression::Type::RFrom, {}};
  case Lexeme::Type::RFetch:
    return Expression{Expression::Type::RFetch, {}};

  case Lexeme::Type::Store:
    return Expression{Expression::Type::Store, {}};
  case Lexeme::Type::Fetch:
    return Expression{Expression::Type::Fetch, {}};
  case Lexeme::Type::CStore:
    return Expression{Expression::Type::CStore, {}};
  case Lexeme::Type::CFetch:
    return Expression{Expression::Type::CFetch, {}};
  case Lexeme::Type::Alloc:
    return Expression{Expression::Type::Alloc, {}};
  case Lexeme::Type::Free:
    return Expression{Expression::Type::Free, {}};

  case Lexeme::Type::DotS:
    return Expression{Expression::Type::DotS, {}};
  case Lexeme::Type::Bye:
    return Expression{Expression::Type::Bye, {}};

  case Lexeme::Type::Col:
    return parseDefineWord(source);
  case Lexeme::Type::Semi:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected semicolon\n";
    exit(EXIT_FAILURE);

  case Lexeme::Type::If: {
    std::vector<Expression> lexemes;
    return parseIf(source, lexemes);
  }
  case Lexeme::Type::Then:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected THEN\n";
    exit(EXIT_FAILURE);
  case Lexeme::Type::Else:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected ELSE\n";
    exit(EXIT_FAILURE);

  case Lexeme::Type::Begin: {
    std::vector<Expression> lexemes;
    return parseBegin(source, lexemes);
  }
  case Lexeme::Type::Until:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected UNTIL\n";
    exit(EXIT_FAILURE);
  case Lexeme::Type::While:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected WHILE\n";
    exit(EXIT_FAILURE);
  case Lexeme::Type::Repeat:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected REPEAT\n";
    exit(EXIT_FAILURE);
  case Lexeme::Type::Again:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected AGAIN\n";
    exit(EXIT_FAILURE);
  }

  std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected\n";
  exit(EXIT_FAILURE);
}
