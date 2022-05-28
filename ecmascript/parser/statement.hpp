#pragma once
#ifndef LIBECMASCRIPT_PARSER_STATEMENT_HPP
#define LIBECMASCRIPT_PARSER_STATEMENT_HPP

#include "expression.hpp"

#include <memory>
#include <variant>
#include <vector>

struct EmptyStatement;
struct BlockStatement;
struct VariableStatement;
struct ExpressionStatement;
struct IfStatement;
struct IterationStatement;
struct ContinueStatement;
struct BreakStatement;
struct ReturnStatement;
struct WithStatement;
struct LabelledStatement;
struct SwitchStatement;
struct ThrowStatement;
struct TryStatement;
struct DebuggerStatement;

struct Statement;

struct VariableDeclaration {};

using StatementList = std::vector<Statement>;
using VariableDeclarationList = std::vector<VariableDeclaration>;

struct EmptyStatement : std::monostate
{
};

struct BlockStatement
{
    StatementList statements;
};

struct VariableStatement
{    
    VariableDeclarationList variableDeclarations;
};

struct ExpressionStatement
{
    Expression expression;
};

struct IfStatement
{
    Expression ifExpression;
    std::unique_ptr<Statement> ifStatement = std::make_unique<Statement>();
    std::unique_ptr<Statement> elseStatement = std::make_unique<Statement>();

};

struct IterationStatement
{
};

struct ContinueStatement
{
};

struct BreakStatement
{
};

struct ReturnStatement
{
};

struct WithStatement
{
    Expression expression;
    std::unique_ptr<Statement> statement = std::make_unique<Statement>();
};

struct LabelledStatement
{
};

struct SwitchStatement
{
};

struct ThrowStatement
{
};

struct TryStatement
{
    StatementList tryBlock;
    Identifier catchIdentifier;
    StatementList catchBlock;
    StatementList finallyBlock;
};

struct DebuggerStatement
{
};

struct Statement
{
    std::variant<
        std::monostate,        
        EmptyStatement,
        BlockStatement,
        VariableStatement,
        ExpressionStatement,
        IfStatement,
        IterationStatement,
        ContinueStatement,
        BreakStatement,
        ReturnStatement,
        WithStatement,
        LabelledStatement,
        SwitchStatement,
        ThrowStatement,
        TryStatement,
        DebuggerStatement
    > data;


    template <typename T>
    constexpr bool holds() const noexcept
    {
        return std::holds_alternative<T>(data);
    }

    constexpr bool empty() const noexcept
    {
        return std::holds_alternative<std::monostate>(data);
    }
    
    template <typename T>
    Statement& operator=(T&& other)
    {
        data = std::move(other);
        return *this;
    }

};


#endif



// struct VariableStatement
// {
// };

// struct EmptyStatement
// {
// };
// struct ExpressionStatement
// {
//     Expression expression;
// };
// struct IfStatement
// {
//     Expression expression;
//     std::unique_ptr<Statement> statement = std::make_unique<Statement>();
//     std::unique_ptr<Statement> elseStatement = std::make_unique<Statement>();
// };
// struct IterationStatement
// {
// };
// struct ContinueStatement
// {
// };
// struct BreakStatement
// {
// };
// struct ReturnStatement
// {
//     Expression expression;
// };
// struct WithStatement
// {
//     Expression expression;
//     std::unique_ptr<Statement> statement = std::make_unique<Statement>();
// };
// struct SwitchStatement
// {
// };
// struct LabelledStatement
// {
// };
// struct ThrowStatement
// {
// };
// struct CatchBlock
// {
//     Identifier identifier;
//     StatementList block;
// };
// struct TryStatement
// {
//     StatementList tryBlock;
//     Identifier catchIdentifier;
//     StatementList catchBlock;
//     StatementList finallyBlock;
// };

// struct DebuggerStatement
// {
// };
