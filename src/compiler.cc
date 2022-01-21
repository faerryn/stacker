#include "compiler.hh"

#include <iostream>
#include <optional>
#include <string>

#include "parser.hh"

void Compiler::compileBody(const std::vector<Expression> &body,
                           std::string &destination) {
  for (const Expression &expr : body) {
    compileExpression(expr, destination);
  }
}

void Compiler::compile(std::istream &source) {
  std::optional<Expression> expression;
  while ((expression = parse(source))) {
    compileExpression(*expression, mainSection);
  }
}

void Compiler::compileExpression(const Expression &expression,
                                 std::string &destination) {
  switch (expression.type) {

  case Expression::Type::Number:
    destination += "// Number\n"
                   "parameterStack.push(" +
                   std::to_string(std::get<std::int64_t>(expression.data)) +
                   ");\n";
    break;
  case Expression::Type::String: {
    const std::string &str = std::get<std::string>(expression.data);
    destination += "// String\n"
                   "{\n"
                   "std::uint8_t *const addr = new std::uint8_t[" +
                   std::to_string(str.size()) + "];\n";
    for (std::size_t i = 0; i < str.size(); ++i) {
      destination += "addr[" + std::to_string(i) +
                     "] = " + std::to_string(int(str[i])) + ";\n";
    }
    destination +
        "parameterStack.push(reinterpret_cast<std::int64_t>(addr));\n"
        "parameterStack.push(" +
        std::to_string(str.size()) +
        ");\n"
        "}\n";
  } break;
  case Expression::Type::Word: {
    const std::string &word = std::get<std::string>(expression.data);
    const auto &find = dictionary.find(word);
    const int name = find->second;
    if (find != dictionary.end()) {
      destination += "// Word " + word +
                     "\n"
                     "word_" +
                     std::to_string(name) + "();\n";
    } else {
      std::cerr << __FILE__ << ":" << __LINE__ << ": unknown word: " << word
                << "\n";
      exit(EXIT_FAILURE);
    }
  } break;

  case Expression::Type::Add:
    destination += "// Add\n"
                   "{\n"
                   "const std::int64_t b = parameterStack.pop();\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "parameterStack.push(a + b);\n"
                   "}\n";
    break;
  case Expression::Type::Sub:
    destination += "// Sub\n"
                   "{\n"
                   "const std::int64_t b = parameterStack.pop();\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "parameterStack.push(a - b);\n"
                   "}\n";
    break;
  case Expression::Type::Mul:
    destination += "// Mul\n"
                   "{\n"
                   "const std::int64_t b = parameterStack.pop();\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "parameterStack.push(a * b);\n"
                   "}\n";
    break;
  case Expression::Type::Div:
    destination += "// Div\n"
                   "{\n"
                   "const std::int64_t b = parameterStack.pop();\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "parameterStack.push(a / b);\n"
                   "}\n";
    break;
  case Expression::Type::Rem:
    destination += "// Rem\n"
                   "{\n"
                   "const std::int64_t b = parameterStack.pop();\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "parameterStack.push(a % b);\n"
                   "}\n";
    break;
  case Expression::Type::Mod:
    destination += "// Mod\n"
                   "{\n"
                   "const std::int64_t b = parameterStack.pop();\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "parameterStack.push((a % b + b) % b);\n"
                   "}\n";
    break;

  case Expression::Type::More:
    destination += "// More\n"
                   "{\n"
                   "const std::int64_t b = parameterStack.pop();\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "parameterStack.push(boolToInt64(a > b));\n"
                   "}\n";
    break;
  case Expression::Type::Less:
    destination += "// Less\n"
                   "{\n"
                   "const std::int64_t b = parameterStack.pop();\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "parameterStack.push(boolToInt64(a < b));\n"
                   "}\n";
    break;
  case Expression::Type::Equal:
    destination += "// Equals\n"
                   "{\n"
                   "const std::int64_t b = parameterStack.pop();\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "parameterStack.push(boolToInt64(a == b));\n"
                   "}\n";
    break;
  case Expression::Type::NotEqual:
    destination += "// NotEquals\n"
                   "{\n"
                   "const std::int64_t b = parameterStack.pop();\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "parameterStack.push(boolToInt64(a != b));\n"
                   "}\n";
    break;

  case Expression::Type::And:
    destination += "// And\n"
                   "{\n"
                   "const std::int64_t b = parameterStack.pop();\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "parameterStack.push(a & b);\n"
                   "}\n";
    break;
  case Expression::Type::Or:
    destination += "// Or\n"
                   "{\n"
                   "const std::int64_t b = parameterStack.pop();\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "parameterStack.push(a | b);\n"
                   "}\n";
    break;
  case Expression::Type::Inv:
    destination += "// Inverse\n"
                   "parameterStack.push(~parameterStack.pop());\n";
    break;

  case Expression::Type::Emit:
    destination += "// Emit\n"
                   "std::cout.put(char(parameterStack.pop()));\n";
    break;
  case Expression::Type::Key:
    destination += "// Key\n"
                   "parameterStack.push(std::cin.get());\n";
    break;

  case Expression::Type::Dup:
    destination += "// Dup\n"
                   "{\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "parameterStack.push(a);\n"
                   "parameterStack.push(a);\n"
                   "}\n";
    break;
  case Expression::Type::Drop:
    destination += "// Drop\n"
                   "parameterStack.pop();\n";
    break;
  case Expression::Type::Swap:
    destination += "// Swap\n"
                   "{\n"
                   "const std::int64_t b = parameterStack.pop();\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "parameterStack.push(b);\n"
                   "parameterStack.push(a);\n"
                   "}\n";
    break;
  case Expression::Type::Over:
    destination += "// Over\n"
                   "{\n"
                   "const std::int64_t b = parameterStack.pop();\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "parameterStack.push(a);\n"
                   "parameterStack.push(b);\n"
                   "parameterStack.push(a);\n"
                   "}\n";
    break;
  case Expression::Type::Rot:
    destination += "// Rot\n"
                   "{\n"
                   "const std::int64_t c = parameterStack.pop();\n"
                   "const std::int64_t b = parameterStack.pop();\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "parameterStack.push(b);\n"
                   "parameterStack.push(c);\n"
                   "parameterStack.push(a);\n"
                   "}\n";
    break;

  case Expression::Type::ToR:
    destination += "// ToR\n"
                   "returnStack.push(parameterStack.pop());\n";
    break;
  case Expression::Type::RFrom:
    destination += "// RFrom\n"
                   "parameterStack.push(returnStack.pop());\n";
    break;
  case Expression::Type::RFetch:
    destination += "// RFetch\n"
                   "{\n"
                   "const std::int64_t a = returnStack.pop();\n"
                   "returnStack.push(a);\n"
                   "parameterStack.push(a);\n"
                   "}\n";
    break;

  case Expression::Type::Store:
    destination += "// Store\n"
                   "{\n"
                   "const std::int64_t b = parameterStack.pop();\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "*reinterpret_cast<std::int64_t *>(b) = a;\n"
                   "}\n";
    break;
  case Expression::Type::Fetch:
    destination +=
        "// Fetch\n"
        "parameterStack.push(\n"
        "*reinterpret_cast<std::int64_t *>(parameterStack.pop()));\n";
    break;
  case Expression::Type::CStore:
    destination += "// CStore\n"
                   "{\n"
                   "const std::int64_t b = parameterStack.pop();\n"
                   "const std::int64_t a = parameterStack.pop();\n"
                   "*reinterpret_cast<char *>(b) = char(a);\n"
                   "}\n";
    break;
  case Expression::Type::CFetch:
    destination += "// CFetch\n"
                   "parameterStack.push(*reinterpret_cast<char *>("
                   "parameterStack.pop()"
                   "));\n";
    break;
  case Expression::Type::Alloc:
    destination +=
        "// Alloc\n"
        "{\n"
        "const std::int64_t size = parameterStack.pop();\n"
        "std::uint8_t *const addr = new std::uint8_t[size];\n"
        "parameterStack.push(reinterpret_cast<std::int64_t>(addr));\n"
        "}\n";
    break;
  case Expression::Type::Free:
    destination + "// Free\n"
                  "{\n"
                  "std::uint8_t *const addr =\n"
                  "reinterpret_cast<std::uint8_t *>(parameterStack.pop());\n"
                  "delete[] addr;\n"
                  "}\n";
    break;

  case Expression::Type::DotS:
    break;
  case Expression::Type::Bye:
    destination += "// Bye\n"
                   "exit(EXIT_SUCCESS);\n";
    break;

  case Expression::Type::WordDefinition: {
    const Expression::WordDefinition &wordDefinition =
        std::get<Expression::WordDefinition>(expression.data);
    std::string declaration = "// Declare " + wordDefinition.word +
                              "\n"
                              "void word_" +
                              std::to_string(nextDictionaryName) + "();\n";

    if (dictionary.contains(wordDefinition.word)) {
      std::cerr << __FILE__ << ":" << __LINE__
                << ": word already defined: " << wordDefinition.word << "\n";
    }
    dictionary[wordDefinition.word] = nextDictionaryName;

    std::string definition = "// Define " + wordDefinition.word +
                             "\n"
                             "void word_" +
                             std::to_string(nextDictionaryName) + "() {\n";
    compileBody(wordDefinition.body, definition);
    definition += "}\n";
    dictionary[wordDefinition.word] = nextDictionaryName;
    declarationSection += declaration;
    definitionSection += definition;

    ++nextDictionaryName;
  } break;

  case Expression::Type::IfThen: {
    const std::vector<Expression> &body =
        std::get<std::vector<Expression>>(expression.data);
    destination += "// IfThen\n"
                   "if (int64ToBool(parameterStack.pop())) {\n";
    compileBody(body, destination);
    destination += "}\n";
  } break;
  case Expression::Type::IfElseThen: {
    const Expression::IfElse &ifElse =
        std::get<Expression::IfElse>(expression.data);
    destination += "// IfElseThen\n"
                   "if (int64ToBool(parameterStack.pop())) {\n";
    compileBody(ifElse.ifBody, destination);
    destination += "} else {\n";
    compileBody(ifElse.elseBody, destination);
    destination += "}\n";
  } break;

  case Expression::Type::BeginUntil: {
    const std::vector<Expression> &body =
        std::get<std::vector<Expression>>(expression.data);
    destination += "// BeginUntil\n"
                   "do {\n";
    compileBody(body, destination);
    destination += "} while (!int64ToBool(parameterStack.pop()));\n";
  } break;
  case Expression::Type::BeginWhileRepeat: {
    const Expression::BeginWhile &beginWhile =
        std::get<Expression::BeginWhile>(expression.data);
    destination += "// BeginWhileRepeat\n";
    compileBody(beginWhile.condBody, destination);
    destination += "while (int64ToBool(parameterStack.pop())) {\n";
    compileBody(beginWhile.whileBody, destination);
    compileBody(beginWhile.condBody, destination);
    destination += "}\n";
  } break;
  case Expression::Type::BeginAgain: {
    const std::vector<Expression> &body =
        std::get<std::vector<Expression>>(expression.data);
    destination += "// BeginAgain\n"
                   "while (true) {\n";
    compileBody(body, destination);
    destination += "}\n";
  } break;
  }
}

void Compiler::write(std::ostream &destination) {
  destination << "// HEADER\n"
                 "#include <cstdint>\n"
                 "#include <iostream>\n"
                 "#include <vector>\n"
                 "class Stack {\n"
                 "private:\n"
                 "std::vector<std::int64_t> data;\n"
                 "public:\n"
                 "void push(std::int64_t number) { data.push_back(number); }\n"
                 "std::int64_t pop() {\n"
                 "const std::int64_t result = data.back();\n"
                 "data.pop_back();\n"
                 "return result;\n"
                 "}\n"
                 "};\n"
                 "Stack parameterStack;\n"
                 "Stack returnStack;\n"
                 "std::int64_t boolToInt64(bool b) { return b ? ~0 : 0; }\n"
                 "bool int64ToBool(std::int64_t i) { return i != 0; }\n"
              << declarationSection << definitionSection
              << "// BODY\n"
                 "int main(int argc, char** argv) {\n"
              << mainSection
              << "// TAIL\n"
                 "}\n";
}
