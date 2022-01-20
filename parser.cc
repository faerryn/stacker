#include "parser.hh"

#include <cstdlib>
#include <vector>

Expression parseDefWord(LexemeSource &source);
Expression parseDefBody(LexemeSource &source, const std::string &word,
                        std::vector<Lexeme> &body);
Expression parseIf(LexemeSource &source, std::vector<Lexeme> &body);
Expression parseIfElse(LexemeSource &source, const Expression::Body &ifBody,
                       std::vector<Lexeme> &body);
Expression parseBegin(LexemeSource &source, std::vector<Lexeme> &body);
Expression parseBeginWhile(LexemeSource &source, const Expression::Body &cond,
                           std::vector<Lexeme> &body);
Expression parseVariable(LexemeSource &source);
std::vector<Expression> parseAll(LexemeSource &source);

LexemeSource::LexemeSource(std::istream *is) : type(Type::FILE), data(is) {}
LexemeSource::LexemeSource(const std::vector<Lexeme> &lexemes)
    : type(Type::COLLECTION), data(Collection{lexemes, 0}) {}

std::optional<Lexeme> LexemeSource::get() {
  switch (type) {
  case Type::NONE:
    return {};
    break;
  case Type::FILE:
    return lex(*std::get<std::istream *>(data));
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

  std::cerr << "unexpected\n";
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

Expression parseVariable(LexemeSource &source) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    std::cerr << "unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::WORD:
    return Expression{Expression::Type::VARIABLE,
                      std::get<std::string>(lexeme->data)};
    break;
  default:
    std::cerr << "expected word\n";
    exit(EXIT_FAILURE);
    break;
  }
}

Expression parseBeginWhile(LexemeSource &source, const Expression::Body &cond,
                           std::vector<Lexeme> &body) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    std::cerr << "unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::REPEAT: {
    LexemeSource bodySource{body};
    return Expression{Expression::Type::BEGIN_WHILE,
                      Expression::BeginWhile{cond, parseAll(bodySource)}};
  } break;
  default: {
    body.push_back(*lexeme);
    return parseBeginWhile(source, cond, body);
  } break;
  }

  std::cerr << "unexpected\n";
  exit(EXIT_FAILURE);
}

Expression parseBegin(LexemeSource &source, std::vector<Lexeme> &body) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    std::cerr << "unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::UNTIL: {
    LexemeSource bodySource{body};
    return Expression{Expression::Type::BEGIN, parseAll(bodySource)};
  } break;
  case Lexeme::Type::WHILE: {
    LexemeSource condSource{body};
    std::vector<Lexeme> whileBody;
    return parseBeginWhile(source, parseAll(condSource), whileBody);
  } break;
  case Lexeme::Type::AGAIN: {
    LexemeSource bodySource{body};
    return Expression{Expression::Type::BEGIN_AGAIN, parseAll(bodySource)};
  } break;
  default:
    body.push_back(*lexeme);
    return parseBegin(source, body);
    break;
  }

  std::cerr << "unexpected\n";
  exit(EXIT_FAILURE);
}

Expression parseIfElse(LexemeSource &source, const Expression::Body &ifBody,
                       std::vector<Lexeme> &body) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    std::cerr << "unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::THEN: {
    LexemeSource bodySource{body};
    return Expression{Expression::Type::IF_ELSE,
                      Expression::IfElse{ifBody, parseAll(bodySource)}};
  } break;
  default: {
    body.push_back(*lexeme);
    return parseIfElse(source, ifBody, body);
  } break;
  }

  std::cerr << "unexpected\n";
  exit(EXIT_FAILURE);
}

Expression parseIf(LexemeSource &source, std::vector<Lexeme> &body) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    std::cerr << "unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::THEN: {
    LexemeSource bodySource{body};
    return Expression{Expression::Type::IF, parseAll(bodySource)};
  } break;
  case Lexeme::Type::ELSE: {
    LexemeSource bodySource{body};
    std::vector<Lexeme> elseBody;
    return parseIfElse(source, parseAll(bodySource), elseBody);
  }
  default: {
    body.push_back(*lexeme);
    return parseIf(source, body);
  } break;
  }

  std::cerr << "unexpected\n";
  exit(EXIT_FAILURE);
}

Expression parseDefBody(LexemeSource &source, const std::string &word,
                        std::vector<Lexeme> &body) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    std::cerr << "unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::SEMICOL: {
    LexemeSource bodySource{body};
    return Expression{Expression::Type::DEF,
                      Expression::Def{word, parseAll(bodySource)}};
  } break;
  default: {
    body.push_back(*lexeme);
    return parseDefBody(source, word, body);
  } break;
  }

  std::cerr << "unexpected\n";
  exit(EXIT_FAILURE);
}

Expression parseDefWord(LexemeSource &source) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    std::cerr << "unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::WORD: {
    const std::string &word = std::get<std::string>(lexeme->data);
    std::vector<Lexeme> body;
    return parseDefBody(source, word, body);
  } break;
  default:
    std::cerr << "expected WORD\n";
    exit(EXIT_FAILURE);
    break;
  }

  std::cerr << "unexpected\n";
  exit(EXIT_FAILURE);
}

std::optional<Expression> parse(LexemeSource &source) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    return {};
  }

  switch (lexeme->type) {

  case Lexeme::Type::NUM:
    return Expression{Expression::Type::NUM,
                      std::get<std::int64_t>(lexeme->data)};
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

  case Lexeme::Type::AND:
    return Expression{Expression::Type::AND, {}};
    break;
  case Lexeme::Type::OR:
    return Expression{Expression::Type::OR, {}};
    break;
  case Lexeme::Type::INVERT:
    return Expression{Expression::Type::INVERT, {}};
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
  case Lexeme::Type::SWAP:
    return Expression{Expression::Type::SWAP, {}};
    break;
  case Lexeme::Type::OVER:
    return Expression{Expression::Type::OVER, {}};
    break;
  case Lexeme::Type::ROT:
    return Expression{Expression::Type::ROT, {}};
    break;

  case Lexeme::Type::RPUT:
    return Expression{Expression::Type::RPUT, {}};
    break;
  case Lexeme::Type::RGET:
    return Expression{Expression::Type::RGET, {}};
    break;

  case Lexeme::Type::COL:
    return parseDefWord(source);
    break;
  case Lexeme::Type::SEMICOL:
    std::cerr << "unexpected semicolon\n";
    exit(EXIT_FAILURE);
    break;

  case Lexeme::Type::IF: {
    std::vector<Lexeme> lexemes;
    return parseIf(source, lexemes);
  } break;
  case Lexeme::Type::THEN:
    std::cerr << "unexpected THEN\n";
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::ELSE:
    std::cerr << "unexpected ELSE\n";
    exit(EXIT_FAILURE);
    break;

  case Lexeme::Type::BEGIN: {
    std::vector<Lexeme> lexemes;
    return parseBegin(source, lexemes);
  } break;
  case Lexeme::Type::UNTIL:
    std::cerr << "unexpected UNTIL\n";
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::WHILE:
    std::cerr << "unexpected WHILE\n";
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::REPEAT:
    std::cerr << "unexpected REPEAT\n";
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::AGAIN:
    std::cerr << "unexpected AGAIN\n";
    exit(EXIT_FAILURE);
    break;

  case Lexeme::Type::VARIABLE:
    return parseVariable(source);
    break;
  case Lexeme::Type::STORE:
    return Expression{Expression::Type::STORE, {}};
    break;
  case Lexeme::Type::FETCH:
    return Expression{Expression::Type::FETCH, {}};
    break;

  case Lexeme::Type::DEBUG:
    return Expression{Expression::Type::DEBUG, {}};
    break;
  case Lexeme::Type::BYE:
    return Expression{Expression::Type::BYE, {}};
    break;
  }

  std::cerr << "unexpected\n";
  exit(EXIT_FAILURE);
}
