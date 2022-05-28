#pragma once
#ifndef LIBECMASCRIPT_PARSER_EXPRESSION_HPP
#define LIBECMASCRIPT_PARSER_EXPRESSION_HPP

#include <variant>

struct EmptyExpression;
struct PrimaryExpression;
struct This;
struct Identifier;
struct Literal;
struct ArrayLiteral;
struct ObjectLiteral;

using Expression = std::variant<
    EmptyExpression,
    This,
    Identifier,
    Literal,
    ArrayLiteral,
    ObjectLiteral
>;

struct EmptyExpression : std::monostate
{
};

struct PrimaryExpression
{
};

struct This : PrimaryExpression
{
};

struct Identifier : PrimaryExpression
{
};

struct Literal : PrimaryExpression
{    
};

struct ArrayLiteral : PrimaryExpression
{    
};

struct ObjectLiteral : PrimaryExpression
{    
};



#endif