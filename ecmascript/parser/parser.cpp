#include "parser.hpp"
#include "statement.hpp"

#include <string>
#include <variant>
#include <vector>



struct Token
{

    std::string source;
    Token(const char *source) : source(source)
    {
    }
    Token(std::string source) : source(source)
    {
    }

    bool operator==(const std::string &what) const
    {
        return source == what;
    }

    bool operator!=(const char *what) const
    {
        return source != what;
    }

    bool operator!=(const Token &what) const
    {
        return source != what;
    }
};
struct Lexer
{
    std::vector<Token> tokens;

    Lexer(std::vector<Token> tokens) : tokens(tokens)
    {
    }

    auto begin()
    {
        return tokens.begin();
    }
    auto end()
    {
        return tokens.end();
    }
};



auto match(auto &&it, auto end, const auto &what)
{
    if (it == end || *it != what)
        return false;
    return ++it, true;
}

auto parse_statement(auto &&it, auto end, auto &result)
{
    // return parse_block(it, end, result)
    //     || parse_variable_statement(it, end, result)
    //     || parse_empty_statement(it, end, result);

    return parse_block_statement(it, end, result) || parse_variable_statement(it, end, result) ||
           parse_empty_statement(it, end, result) || parse_debugger_statement(it, end, result);
}

auto parse_block_statement(auto &&it, auto end, auto &result)
{
    auto jt = it;
    if (!match(jt, end, "{"))
        return false;
    StatementList statements;
    parse_statement_list(jt, end, statements);
    if (!match(jt, end, "}"))
        return false;
    result = BlockStatement{std::move(statements)};
    it = jt;
    return true;
}

auto parse_variable_statement(auto &&it, auto end, auto &result)
{
    auto jt = it;
    if (!match(jt, end, "var"))
        return false;
    VariableDeclarationList variableDeclarations;

    if (!match(jt, end, ";"))
        return false;
    result = VariableStatement{std::move(variableDeclarations)};
    it = jt;
    return true;
}

auto parse_empty_statement(auto &&it, auto end, auto &result)
{
    auto jt = it;
    if (!match(jt, end, ";"))
        return false;
    result = std::move(EmptyStatement{});
    it = jt;
    return true;
}

auto parse_expression_statement(auto &&it, auto end, auto &result)
{
    return false;
}

auto parse_if_statement(auto &&it, auto end, auto &result)
{
    if (!match(it, end, "if"))
        return false;

    auto stmt = IfStatement{};
    if (!match(it, end, "("))
        throw SyntaxError("Expected (");
    if (!parse_expression(it, end, stmt.ifExpression))
        throw SyntaxError("Expected Expression");
    if (!match(it, end, ")"))
        throw SyntaxError("Expected )");
    if (!parse_statement(it, end, *stmt.ifStatement.get()))
        throw SyntaxError("Expected Statement");
    if (match(it, end, "else") && !parse_statement(it, end, *stmt.elseStatement.get()))
        throw SyntaxError("Expected Statement");
    result = std::move(stmt);
    return true;
}

auto parse_iteration_statement(auto &&it, auto end, auto &result)
{
    return false;
}

auto parse_continue_statement(auto &&it, auto end, auto &result)
{
    if (!match(it, end, "continue"))
        return false;
    auto stmt = ContinueStatement{};
    if (!match(it, end, ";"))
        throw SyntaxError("Expected ;");
    result = stmt;
    return true;
}

auto parse_break_statement(auto &&it, auto end, auto &result)
{
    if (!match(it, end, "break"))
        return false;
    auto stmt = BreakStatement{};
    if (!match(it, end, ";"))
        throw SyntaxError("Expected ;");
    result = stmt;
    return true;
}

auto parse_return_statement(auto &&it, auto end, auto &result)
{
    if (!match(it, end, "return"))
        return false;
    auto stmt = ReturnStatement{};
    if (!match(it, end, ";"))
        throw SyntaxError("Expected ;");
    result = stmt;
    return true;
}

auto parse_with_statement(auto &&it, auto end, auto &result)
{
    if (!match(it, end, "with"))
        return false;
    auto stmt = WithStatement{};
    if (!match(it, end, "("))
        throw SyntaxError("Expected (");
    if (!parse_expression(it, end, stmt.expression))
        throw SyntaxError("Expected Expression");
    if (!match(it, end, ")"))
        throw SyntaxError("Expected )");
    if (!parse_statement(it, end, *stmt.statement))
        throw SyntaxError("Expected Statement");
    result = stmt;
    return true;
}
// ...

auto parse_try_statement(auto &&it, auto end, auto &result)
{
    auto jt = it;
    if (!match(jt, end, "try"))
        return false;
    auto stmt = TryStatement{};
    auto hasCatchBlock = parse_catch(jt, end, stmt);
    auto hasFinallyBlock = parse_finally(jt, end, stmt.finallyBlock);
    if (!hasCatchBlock && !hasFinallyBlock)
        throw SyntaxError("Expected Catch or Finally");
    result = std::move(stmt);
    it = jt;
    return true;
}

auto parse_debugger_statement(auto &&it, auto end, auto &result)
{
    auto jt = it;
    if (!match(jt, end, "debugger"))
        return false;
    if (!match(jt, end, ";"))
        return false;
    result = DebuggerStatement{};
    it = jt;
    return true;
}

// ---

struct Block
{
    StatementList statements;
};

auto parse_block(auto &&it, auto end, auto &result)
{
    auto jt = it;
    if (!match(jt, end, "{"))
        return false;
    StatementList statements;
    parse_statement_list(jt, end, statements);
    if (!match(jt, end, "}"))
        return false;
    result = std::move(statements);
    it = jt;
    return true;
}

void parse_statement_list(auto &&it, auto end, auto &result)
{
    while (true)
    {
        Statement stmt;
        if (!parse_statement(it, end, stmt))
            return;
        result.emplace_back(std::move(stmt));
    }
}

auto parse_identifier(auto &&it, auto end, auto &result)
{
    return false;
}

auto parse_catch(auto &&it, auto end, auto &result)
{
    auto jt = it;
    if (!match(jt, end, "catch"))
        return false;
    if (!match(jt, end, "("))
        throw SyntaxError("Expected (");
    if (!parse_identifier(jt, end, result.catchIdentifier))
        throw SyntaxError("Expected Identifier");
    if (!match(jt, end, ")"))
        throw SyntaxError("Expected )");
    if (!parse_block(jt, end, result.catchBlock))
        throw SyntaxError("Expected Block");
    return true;
}

auto parse_finally(auto &&it, auto end, auto &result)
{
    auto jt = it;
    if (!match(jt, end, "finally"))
        return false;
    if (!parse_block(jt, end, result))
        throw SyntaxError("Expected Block");
    return true;
}