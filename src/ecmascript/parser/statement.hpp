#ifndef __ECMASCRIPT_PARSER_STATEMENT_HPP
#define __ECMASCRIPT_PARSER_STATEMENT_HPP

#include <ecmascript/parser/expression.hpp>

#include <memory>
#include <variant>
#include <vector>

namespace ecmascript
{

struct Identifier
{
    std::string value;
    bool operator==(const Identifier &other) const = default;
};

struct BlockStatement;
struct VariableStatement;
struct EmptyStatement;
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

using Statement = std::variant<std::monostate, BlockStatement, VariableStatement, EmptyStatement>;

// using Statement =
//     std::variant<std::monostate, BlockStatement, VariableStatement, EmptyStatement, ExpressionStatement, IfStatement,
//                  IterationStatement, ContinueStatement, BreakStatement, ReturnStatement, WithStatement,
//                  LabelledStatement, SwitchStatement, ThrowStatement, TryStatement, DebuggerStatement>;

struct StatementRef
{
    std::unique_ptr<Statement> data;
    StatementRef() = default;
    StatementRef(StatementRef &&) = default;
    StatementRef(Statement &&);
    StatementRef &operator=(Statement &&);
    operator Statement &();

    bool operator==(const StatementRef &other) const;
};

struct BlockStatement
{
    std::vector<Statement> statements;
    bool operator==(const BlockStatement &other) const;
};

struct VariableDeclaration
{
    // Identifier identifier;
    // Expression initialiser;
    bool operator==(const VariableDeclaration &other) const = default;
};

struct VariableStatement
{
    std::vector<VariableDeclaration> variableDeclarations;
    bool operator==(const VariableStatement &other) const = default;
};

struct EmptyStatement
{
    bool operator==(const EmptyStatement &other) const = default;
};

struct ExpressionStatement
{
    Expression expression;
    bool operator==(const ExpressionStatement &other) const = default;
};

struct IfStatement
{
    Expression expression;
    StatementRef ifStatement;
    StatementRef elseStatement;
    bool operator==(const IfStatement &other) const = default;
};

struct IterationStatement
{
    StatementRef statement;
    Expression initExpression, preExpression, postExpression;
    bool operator==(const IterationStatement &other) const = default;
};

struct ContinueStatement
{
    Identifier identifier;
    bool operator==(const ContinueStatement &other) const = default;
};

struct BreakStatement
{
    Identifier identifier;
    bool operator==(const BreakStatement &other) const = default;
};

struct ReturnStatement
{
    Expression expression;
    bool operator==(const ReturnStatement &other) const = default;
};

struct WithStatement
{
    Expression expression;
    StatementRef statement;
    bool operator==(const WithStatement &other) const = default;
};

struct LabelledStatement
{
    Identifier identifier;
    StatementRef statement;
    bool operator==(const LabelledStatement &other) const = default;
};

struct CaseClause
{
    Expression expression;
    std::vector<Statement> statements;
    bool operator==(const CaseClause &other) const = default;
};

struct DefaultClause
{
    std::vector<Statement> statements;
    bool operator==(const DefaultClause &other) const = default;
};

struct SwitchStatement
{
    Expression expression;
    DefaultClause defaultClause;
    std::vector<CaseClause> caseClauses;
    bool operator==(const SwitchStatement &other) const = default;
};

struct ThrowStatement
{
    Expression expression;
    bool operator==(const ThrowStatement &other) const = default;
};

struct TryStatement
{
    BlockStatement tryBlock, catchBlock, finallyBlock;
    Identifier catchIdentifier;
    bool operator==(const TryStatement &other) const = default;
};

struct DebuggerStatement
{
    bool operator==(const DebuggerStatement &other) const = default;
};

} // namespace ecmascript

bool operator==(const ecmascript::Statement &lhs, const ecmascript::Statement &rhs);

#endif