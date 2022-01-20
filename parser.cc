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

LexemeSource::LexemeSource(std::istream *is) : type(Type::Stream), data(is) {}
LexemeSource::LexemeSource(const std::vector<Lexeme> &lexemes)
    : type(Type::Collection), data(Collection{lexemes, 0}) {}

std::optional<Lexeme> LexemeSource::get() {
  switch (type) {
  case Type::Stream:
    return lex(*std::get<std::istream *>(data));
    break;
  case Type::Collection: {
    Collection &collection = std::get<Collection>(data);
    if (collection.index < collection.lexemes.size()) {
      ++collection.index;
      return collection.lexemes[collection.index - 1];
    } else {
      return {};
    }
  } break;
  }

  std::cerr << __FILE__ << ":" << __LINE__ << ": unrechable\n";
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

Expression parseBeginWhile(LexemeSource &source, const Expression::Body &cond,
                           std::vector<Lexeme> &body) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::Repeat: {
    LexemeSource bodySource{body};
    return Expression{Expression::Type::BeginWhileRepeat,
                      Expression::BeginWhile{cond, parseAll(bodySource)}};
  } break;
  default: {
    body.push_back(*lexeme);
    return parseBeginWhile(source, cond, body);
  } break;
  }

  std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected\n";
  exit(EXIT_FAILURE);
}

Expression parseBegin(LexemeSource &source, std::vector<Lexeme> &body) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::Until: {
    LexemeSource bodySource{body};
    return Expression{Expression::Type::BeginUntil, parseAll(bodySource)};
  } break;
  case Lexeme::Type::While: {
    LexemeSource condSource{body};
    std::vector<Lexeme> whileBody;
    return parseBeginWhile(source, parseAll(condSource), whileBody);
  } break;
  case Lexeme::Type::Again: {
    LexemeSource bodySource{body};
    return Expression{Expression::Type::BeginAgain, parseAll(bodySource)};
  } break;
  default:
    body.push_back(*lexeme);
    return parseBegin(source, body);
    break;
  }

  std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected\n";
  exit(EXIT_FAILURE);
}

Expression parseIfElse(LexemeSource &source, const Expression::Body &ifBody,
                       std::vector<Lexeme> &body) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::Then: {
    LexemeSource bodySource{body};
    return Expression{Expression::Type::IfElseThen,
                      Expression::IfElse{ifBody, parseAll(bodySource)}};
  } break;
  default: {
    body.push_back(*lexeme);
    return parseIfElse(source, ifBody, body);
  } break;
  }

  std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected\n";
  exit(EXIT_FAILURE);
}

Expression parseIf(LexemeSource &source, std::vector<Lexeme> &body) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::Then: {
    LexemeSource bodySource{body};
    return Expression{Expression::Type::IfThen, parseAll(bodySource)};
  } break;
  case Lexeme::Type::Else: {
    LexemeSource bodySource{body};
    std::vector<Lexeme> elseBody;
    return parseIfElse(source, parseAll(bodySource), elseBody);
  }
  default: {
    body.push_back(*lexeme);
    return parseIf(source, body);
  } break;
  }

  std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected\n";
  exit(EXIT_FAILURE);
}

Expression parseDefBody(LexemeSource &source, const std::string &word,
                        std::vector<Lexeme> &body) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::Semi: {
    LexemeSource bodySource{body};
    return Expression{Expression::Type::Define,
                      Expression::Def{word, parseAll(bodySource)}};
  } break;
  case Lexeme::Type::Col:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected col\n";
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::Rec:
    body.push_back(Lexeme{Lexeme::Type::Word, word});
    return parseDefBody(source, word, body);
    break;
  default: {
    body.push_back(*lexeme);
    return parseDefBody(source, word, body);
  } break;
  }

  std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected\n";
  exit(EXIT_FAILURE);
}

Expression parseDefWord(LexemeSource &source) {
  std::optional<Lexeme> lexeme = source.get();
  if (!lexeme) {
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected EOF\n";
    exit(EXIT_FAILURE);
  }

  switch (lexeme->type) {
  case Lexeme::Type::Word: {
    const std::string &word = std::get<std::string>(lexeme->data);
    std::vector<Lexeme> body;
    return parseDefBody(source, word, body);
  } break;
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": expected WORD\n";
    exit(EXIT_FAILURE);
    break;
  }

  std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected\n";
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

  case Lexeme::Type::Col:
    return parseDefWord(source);
    break;
  case Lexeme::Type::Rec:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected recurse\n";
    exit(EXIT_FAILURE);
    break;
  case Lexeme::Type::Semi:
    std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected semicolon\n";
    exit(EXIT_FAILURE);
    break;

  case Lexeme::Type::If: {
    std::vector<Lexeme> lexemes;
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
    std::vector<Lexeme> lexemes;
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
  }

  std::cerr << __FILE__ << ":" << __LINE__ << ": unexpected\n";
  exit(EXIT_FAILURE);
}
