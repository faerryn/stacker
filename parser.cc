#include "parser.hh"

#include <cstdio>
#include <cstdlib>
#include <vector>

std::optional<Lexeme> LexemeSource::get() {
  switch (type) {
  case Type::NONE:
    return {};
    break;
  case Type::FILE:
    return lex(std::get<FILE *>(data));
    break;
  case Type::COLLECTION: {
    Collection &collection = std::get<Collection>(data);
    if (collection.index < collection.lexemes.size()) {
      const size_t lastIndex = collection.index;
      ++collection.index;
      return collection.lexemes[lastIndex];
    } else {
      return {};
    }
  } break;
  }

  fprintf(stderr, "unexpected\n");
  exit(EXIT_FAILURE);
}

Expression::Body parseAll(LexemeSource &source) {
  Expression::Body body;
  std::optional<Expression> expr;
  while ((expr = parse(source))) {
    body.push_back(*expr);
  }
  return body;
}

std::optional<Expression> parseBeginWhile(LexemeSource &source,
                                          const Expression::Body &cond,
                                          std::vector<Lexeme> &body) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    fprintf(stderr, "unexpected EOF\n");
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::REPEAT: {
    LexemeSource bodySource{body.begin(), body.end()};
    return Expression{Expression::Type::BEGIN_WHILE,
                      Expression::BeginWhile{cond, parseAll(bodySource)}};
  } break;
  default: {
    body.push_back(*lexeme);
    return parseBeginWhile(source, cond, body);
  } break;
  }

  fprintf(stderr, "unexpected\n");
  exit(EXIT_FAILURE);
}

std::optional<Expression> parseBegin(LexemeSource &source,
                                     std::vector<Lexeme> &body) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    fprintf(stderr, "unexpected EOF\n");
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::UNTIL: {
    LexemeSource bodySource{body.begin(), body.end()};
    return Expression{Expression::Type::BEGIN, parseAll(bodySource)};
  } break;
  case Lexeme::Type::WHILE: {
    LexemeSource condSource{body.begin(), body.end()};
    std::vector<Lexeme> whileBody;
    return parseBeginWhile(source, parseAll(condSource), whileBody);
  } break;
  default:
    body.push_back(*lexeme);
    return parseBegin(source, body);
    break;
  }

  fprintf(stderr, "unexpected\n");
  exit(EXIT_FAILURE);
}

std::optional<Expression> parseIf(LexemeSource &source,
                                  std::vector<Lexeme> &body) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    fprintf(stderr, "unexpected EOF\n");
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::THEN: {
    LexemeSource bodySource{body.begin(), body.end()};
    return Expression{Expression::Type::IF, parseAll(bodySource)};
  } break;
  default: {
    body.push_back(*lexeme);
    return parseIf(source, body);
  } break;
  }

  fprintf(stderr, "unexpected\n");
  exit(EXIT_FAILURE);
}

std::optional<Expression> parseDefBody(LexemeSource &source,
                                       const std::string &word,
                                       std::vector<Lexeme> &body) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    fprintf(stderr, "unexpected EOF\n");
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::SEMICOL: {
    LexemeSource bodySource{body.begin(), body.end()};
    return Expression{Expression::Type::DEF,
                      Expression::Def{word, parseAll(bodySource)}};
  } break;
  default: {
    body.push_back(*lexeme);
    return parseDefBody(source, word, body);
  } break;
  }

  fprintf(stderr, "unexpected\n");
  exit(EXIT_FAILURE);
}

std::optional<Expression> parseDefWord(LexemeSource &source) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    fprintf(stderr, "unexpected EOF\n");
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::WORD: {
    const std::string &word = std::get<std::string>(lexeme->data);
    std::vector<Lexeme> body;
    return parseDefBody(source, word, body);
  } break;
  default:
    fprintf(stderr, "expected WORD\n");
    exit(EXIT_FAILURE);
    break;
  }

  fprintf(stderr, "unexpected\n");
  exit(EXIT_FAILURE);
}

std::optional<Expression> parse(LexemeSource &source) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    return {};
  }

  switch (lexeme->type) {
  case Lexeme::Type::NUM:
    return Expression{Expression::Type::NUM, std::get<int64_t>(lexeme->data)};
    break;
  case Lexeme::Type::WORD:
    return Expression{Expression::Type::WORD,
                      std::get<std::string>(lexeme->data)};
    break;
  case Lexeme::Type::ADD:
    return Expression{Expression::Type::ADD, {}};
    break;
  case Lexeme::Type::SUB:
    return Expression{Expression::Type::SUB, {}};
    break;
  case Lexeme::Type::MUL:
    return Expression{Expression::Type::MUL, {}};
    break;
  case Lexeme::Type::DIV:
    return Expression{Expression::Type::DIV, {}};
    break;
  case Lexeme::Type::REM:
    return Expression{Expression::Type::REM, {}};
    break;
  case Lexeme::Type::MOD:
    return Expression{Expression::Type::MOD, {}};
    break;
  case Lexeme::Type::GT:
    return Expression{Expression::Type::GT, {}};
    break;
  case Lexeme::Type::LT:
    return Expression{Expression::Type::LT, {}};
    break;
  case Lexeme::Type::EQ:
    return Expression{Expression::Type::EQ, {}};
    break;
  case Lexeme::Type::NEQ:
    return Expression{Expression::Type::NEQ, {}};
    break;
  case Lexeme::Type::DOT:
    return Expression{Expression::Type::DOT, {}};
    break;
  case Lexeme::Type::EMIT:
    return Expression{Expression::Type::EMIT, {}};
    break;
  case Lexeme::Type::DUP:
    return Expression{Expression::Type::DUP, {}};
    break;
  case Lexeme::Type::DROP:
    return Expression{Expression::Type::DROP, {}};
    break;
  case Lexeme::Type::SWITCH:
    return Expression{Expression::Type::SWITCH, {}};
    break;
  case Lexeme::Type::OVER:
    return Expression{Expression::Type::OVER, {}};
    break;
  case Lexeme::Type::ROT:
    return Expression{Expression::Type::ROT, {}};
    break;
  case Lexeme::Type::COL:
    return parseDefWord(source);
    break;
  case Lexeme::Type::SEMICOL:
    fprintf(stderr, "unexpected semicolon\n");
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::IF: {
    std::vector<Lexeme> lexemes;
    return parseIf(source, lexemes);
  } break;
  case Lexeme::Type::THEN:
    fprintf(stderr, "unexpected THEN\n");
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::BEGIN: {
    std::vector<Lexeme> lexemes;
    return parseBegin(source, lexemes);
  } break;
  case Lexeme::Type::UNTIL:
    fprintf(stderr, "unexpected UNTIL\n");
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::WHILE:
    fprintf(stderr, "unexpected WHILE\n");
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::REPEAT:
    fprintf(stderr, "unexpected REPEAT\n");
    exit(EXIT_FAILURE);
    break;
  }

  fprintf(stderr, "unexpected\n");
  exit(EXIT_FAILURE);
}
