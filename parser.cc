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
      ++collection.index;
      return collection.lexemes[collection.index - 1];
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
  case Lexeme::Type::Word:
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
  case Lexeme::Type::Semi: {
    LexemeSource bodySource{body};
    return Expression{Expression::Type::DEF,
                      Expression::Def{word, parseAll(bodySource)}};
  } break;
  case Lexeme::Type::Colon:
    std::cerr << "unexpected col\n";
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::Recurse:
    body.push_back(Lexeme{Lexeme::Type::Word, word});
    return parseDefBody(source, word, body);
    break;
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
  case Lexeme::Type::Word: {
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

  case Lexeme::Type::Number:
    return Expression{Expression::Type::Number,
                      std::get<std::int64_t>(lexeme->data)};
    break;
  case Lexeme::Type::Word:
    return Expression{Expression::Type::Word,
                      std::get<std::string>(lexeme->data)};
    break;
  case Lexeme::Type::String:
    return Expression{Expression::Type::String,
                      std::get<std::string>(lexeme->data)};
    break;

  case Lexeme::Type::Plus:
    return Expression{Expression::Type::Plus, {}};
    break;
  case Lexeme::Type::Minus:
    return Expression{Expression::Type::Minus, {}};
    break;
  case Lexeme::Type::Times:
    return Expression{Expression::Type::Times, {}};
    break;
  case Lexeme::Type::Div:
    return Expression{Expression::Type::Div, {}};
    break;
  case Lexeme::Type::REM:
    return Expression{Expression::Type::REM, {}};
    break;
  case Lexeme::Type::MOD:
    return Expression{Expression::Type::MOD, {}};
    break;

  case Lexeme::Type::less:
    return Expression{Expression::Type::less, {}};
    break;
  case Lexeme::Type::more:
    return Expression{Expression::Type::more, {}};
    break;
  case Lexeme::Type::Equal:
    return Expression{Expression::Type::Equal, {}};
    break;
  case Lexeme::Type::ne:
    return Expression{Expression::Type::ne, {}};
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

  case Lexeme::Type::d:
    return Expression{Expression::Type::d, {}};
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

  case Lexeme::Type::ToR:
    return Expression{Expression::Type::RPUT, {}};
    break;
  case Lexeme::Type::RFrom:
    return Expression{Expression::Type::RGET, {}};
    break;

  case Lexeme::Type::Colon:
    return parseDefWord(source);
    break;
  case Lexeme::Type::Recurse:
    std::cerr << "unexpected recurse\n";
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::Semi:
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
  case Lexeme::Type::Store:
    return Expression{Expression::Type::STORE, {}};
    break;
  case Lexeme::Type::Fetch:
    return Expression{Expression::Type::FETCH, {}};
    break;
  case Lexeme::Type::CStore:
    return Expression{Expression::Type::CSTORE, {}};
    break;
  case Lexeme::Type::CFetch:
    return Expression{Expression::Type::CFETCH, {}};
    break;

  case Lexeme::Type::DotS:
    return Expression{Expression::Type::DEBUG, {}};
    break;
  case Lexeme::Type::BYE:
    return Expression{Expression::Type::BYE, {}};
    break;
  }

  std::cerr << "unexpected\n";
  exit(EXIT_FAILURE);
}
