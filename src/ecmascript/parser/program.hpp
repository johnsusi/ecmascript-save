#ifndef __ECMASCRIPT_PARSER_PROGRAM_HPP
#define __ECMASCRIPT_PARSER_PROGRAM_HPP

#include <ecmascript/parser/statement.hpp>

#include <variant>
#include <vector>

namespace ecmascript
{

struct FunctionDeclaration;
using SourceElement = std::variant<Statement, FunctionDeclaration>;

struct FunctionBody
{
    std::vector<SourceElement> sourceElements;
};

using FormalParameterList = std::vector<Identifier>;

struct FunctionDeclaration : FunctionBody
{
    Identifier identifier;
    FormalParameterList formalParameterList;
};

struct Program : FunctionBody
{
};

} // namespace ecmascript

#endif