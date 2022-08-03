#ifndef __ECMASCRIPT_RUNTIME_EVAL_HPP
#define __ECMASCRIPT_RUNTIME_EVAL_HPP

#include <string_view>
#include <variant>
#include <vector>

struct Value
{
    bool isSource() const;

    operator const std::string_view &() const;
};

struct LexicalEnvironment
{
};

LexicalEnvironment NewDeclarativeEnvironment(LexicalEnvironment *E = nullptr)
{
    return {};
}

struct ExecutionContext
{
    LexicalEnvironment lexical, variable;
    Value thisBinding;
};

struct Statement
{
};

using Program = std::vector<Statement>;

class Lexer
{
  public:
    Lexer(const std::string_view &source);
};

class Parser
{
  public:
    Parser(Lexer lexer);
    Program parse();
};

Value eval(Value source, ExecutionContext *callingContext)
{
    if (!source.isSource())
        return source;

    auto lexer = Lexer{source};
    auto parser = Parser{lexer};
    auto program = parser.parse();

    auto globalEnvironment = LexicalEnvironment{};
    auto globalObject = Value{};

    auto context = ExecutionContext{.lexical = globalEnvironment, .thisBinding = globalObject};

    // TODO: Eval program
}

#endif