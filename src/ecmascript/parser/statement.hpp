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
    Identifier &operator=(std::string v)
    {
        value = v;
        return *this;
    }
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

using Statement =
    std::variant<Empty, BlockStatement, VariableStatement, EmptyStatement, ExpressionStatement, IfStatement,
                 IterationStatement, ContinueStatement, BreakStatement, ReturnStatement, WithStatement,
                 LabelledStatement, SwitchStatement, ThrowStatement, TryStatement, DebuggerStatement>;

using StatementRef = RecursiveWrapper<Statement>;

struct StatementList
{
    std::vector<Statement> statements;
    StatementList(std::initializer_list<Statement> init);
    bool operator==(const StatementList &) const;
    void emplace_back(Statement &&statement);
};

struct BlockStatement
{
    StatementList statements;
    bool operator==(const BlockStatement &) const;
};

struct VariableDeclaration
{
    Identifier identifier;
    Expression initialiser;
    bool operator==(const VariableDeclaration &) const;
};

struct VariableStatement
{
    std::vector<VariableDeclaration> variableDeclarations;
    bool operator==(const VariableStatement &) const;
};

struct EmptyStatement
{
    bool operator==(const EmptyStatement &) const;
};

struct ExpressionStatement
{
    Expression expression;
    bool operator==(const ExpressionStatement &) const;
};

struct IfStatement
{
    Expression expression;
    StatementRef ifStatement, elseStatement;
    bool operator==(const IfStatement &) const;
};

struct IterationStatement
{
    Expression initExpression, preTestExpression, postTestExpression, postExpression;
    StatementRef statement;
    bool operator==(const IterationStatement &) const;
};

struct ContinueStatement
{
    Identifier identifier;
    bool operator==(const ContinueStatement &) const;
};

struct BreakStatement
{
    Identifier identifier;
    bool operator==(const BreakStatement &) const;
};

struct ReturnStatement
{
    Expression expression;
    bool operator==(const ReturnStatement &) const;
};

struct WithStatement
{
    Expression expression;
    StatementRef statement;
    bool operator==(const WithStatement &) const;
};

struct LabelledStatement
{
    Identifier identifier;
    StatementRef statement;
    bool operator==(const LabelledStatement &) const;
};

struct CaseClause
{
    Expression expression;
    StatementList statements;
    bool operator==(const CaseClause &) const;
};

struct DefaultClause
{
    StatementList statements;
    bool operator==(const DefaultClause &) const;
};

struct SwitchStatement
{
    Expression expression;
    DefaultClause defaultClause;
    std::vector<CaseClause> caseClauses;
    bool operator==(const SwitchStatement &) const;
};

struct ThrowStatement
{
    Expression expression;
    bool operator==(const ThrowStatement &) const;
};

struct TryStatement
{
    BlockStatement tryBlock, catchBlock, finallyBlock;
    Identifier catchIdentifier;
    bool operator==(const ecmascript::TryStatement &) const;
};

struct DebuggerStatement
{
    bool operator==(const DebuggerStatement &) const;
};

inline bool StatementList::operator==(const StatementList &) const = default;
inline bool EmptyStatement::operator==(const EmptyStatement &) const = default;
inline bool BlockStatement::operator==(const BlockStatement &) const = default;
inline bool VariableDeclaration::operator==(const VariableDeclaration &) const = default;
inline bool VariableStatement::operator==(const VariableStatement &) const = default;
inline bool ExpressionStatement::operator==(const ExpressionStatement &) const = default;
inline bool IfStatement::operator==(const IfStatement &) const = default;
inline bool IterationStatement::operator==(const IterationStatement &) const = default;
inline bool ContinueStatement::operator==(const ContinueStatement &) const = default;
inline bool BreakStatement::operator==(const BreakStatement &) const = default;
inline bool ReturnStatement::operator==(const ReturnStatement &) const = default;
inline bool WithStatement::operator==(const WithStatement &) const = default;
inline bool LabelledStatement::operator==(const LabelledStatement &) const = default;
inline bool CaseClause::operator==(const CaseClause &) const = default;
inline bool DefaultClause::operator==(const DefaultClause &) const = default;
inline bool SwitchStatement::operator==(const SwitchStatement &) const = default;
inline bool ThrowStatement::operator==(const ThrowStatement &) const = default;
inline bool TryStatement::operator==(const TryStatement &) const = default;
inline bool DebuggerStatement::operator==(const DebuggerStatement &) const = default;

inline StatementList::StatementList(std::initializer_list<Statement> init) : statements(init)
{
}

inline void StatementList::emplace_back(Statement &&statement)
{
    statements.push_back(statement);
}

} // namespace ecmascript

#endif