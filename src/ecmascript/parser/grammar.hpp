#ifndef __ECMASCRIPT_PARSER_GRAMMAR_HPP
#define __ECMASCRIPT_PARSER_GRAMMAR_HPP

#include <ecmascript/lexer/token.hpp>
#include <ecmascript/parser/expression.hpp>
#include <ecmascript/parser/program.hpp>
#include <ecmascript/parser/statement.hpp>

#include <algorithm>
#include <array>
#include <stdexcept>
#include <vector>

namespace ecmascript
{

struct SyntaxError : std::runtime_error
{
    SyntaxError(const char *what, auto it, auto end) : runtime_error(what)
    {
    }
};

auto is_match(auto &&it, auto end, auto &&what)
{
    if (it == end || *it != what)
        return false;
    return ++it, true;
}

auto is_match(auto &&it, auto end, auto &&what, auto &result)
{
    if (it == end || *it != what)
        return false;
    result = *it;
    return ++it, true;
}

auto is_oneof(auto &&it, auto end, const std::vector<Token> &oneof, auto &result)
{
    if (it == end || std::none_of(oneof.begin(), oneof.end(), *it))
        return false;
    result = *it;
    return ++it, true;
}

bool is_match(auto &&it, auto end, bool (Token::*what)() const)
{
    if (it == end || !(it->*what)())
        return false;
    return ++it, true;
}

template <typename... Args> bool lookahead(auto it, auto end, Args &&...what)
{
    return (is_match(it, end, std::forward<Args>(what)) && ...);
}

auto is_line_terminator(auto it, auto end)
{
    return it != end && it->separatedWithLineTerminator();
}

auto is_semicolon(auto &&it, auto end)
{
    // 7.9.1 Rules of Automatic Semicolon Insertion
    if (it == end)
        return true;
    else if (*it == ";" || *it == "}")
        return ++it, true;
    else if (is_line_terminator(it, end))
        return true;
    return false;
}

auto is_this(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "this"))
        return false;
    result = ThisExpression{};
    return true;
}

auto is_identifier(auto &&it, auto end, auto &result)
{
    if (!lookahead(it, end, &Token::isIdentifier))
        return false;
    // result = Identifier{it->value};
    ++it;
    return true;
}

auto is_literal(auto &&it, auto end, auto &result)
{
    return false;
}

auto is_array_literal(auto &&it, auto end, auto &result)
{
    return false;
}

auto is_object_literal(auto &&it, auto end, auto &result)
{
    return false;
}

auto is_grouping(auto &&it, auto end, auto &result)
{
    return false;
}

auto is_primary_expression(auto &&it, auto end, auto &result)
{
    return is_this(it, end, result) || is_identifier(it, end, result) || is_literal(it, end, result) ||
           is_array_literal(it, end, result) || is_object_literal(it, end, result) || is_grouping(it, end, result);
}

auto is_arguments(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "("))
        return false;
    auto temp = Arguments{};
    for (auto expr = Expression{}; is_assignment_expression(it, end, expr);)
    {
        temp.emplace_back(std::move(expr));
        if (!is_match(it, end, ","))
            break;
    }
    if (!is_match(it, end, ")"))
        throw SyntaxError("Expected ')'", it, end);
    result = std::move(temp);
}

auto is_member_expression(auto &&it, auto end, auto &result)
{
    auto temp = Expression{};
    if (!is_primary_expression(it, end, temp) /*|| !is_function_expression(it, end, temp)*/)
        return false;

    while (true)
    {
        if (is_match(it, end, "["))
        {
            auto expression = Expression{};
            if (!is_expression(it, end, expression))
                throw SyntaxError("Expected <Expression>", it, end);
            if (!is_match(it, end, "]"))
                throw SyntaxError("Expected ']", it, end);
            temp = MemberExpression(std::move(temp), std::move(expression));
        }
        else if (is_match(it, end, "."))
        {
            auto identifier = Expression{};
            if (!is_identifier_name(it, end, identifier))
                throw SyntaxError("Expected <IdentifierName>", it, end);
            temp = MemberExpression{std::move(result), std::move(identifier)};
        }
        else
            break;
    }
    result = std::move(temp);
    return true;
}

auto is_new_expression(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "new"))
        return is_member_expression(it, end, result);
    auto temp = NewExpression{};
    if (!is_new_expression(it, end, temp.expression))
        throw SyntaxError("Expected <NewExpression>", it, end);
    is_arguments(it, end, temp.arguments);
    result = std::move(temp);
    return true;
}

auto is_left_hand_side_expression(auto &&it, auto end, auto &result)
{
    auto temp = Expression{};
    if (!is_new_expression(it, end, temp))
        return false;

    auto arguments = Arguments{};
    if (is_arguments(it, end, arguments))
    {
        temp = CallExpression(std::move(temp), std::move(arguments));
    }
    else if (is_match(it, end, "["))
    {
        auto expression = Expression{};
        if (!is_expression(it, end, expression))
            throw SyntaxError("Expected <Expression>", it, end);
        if (!is_match(it, end, "]"))
            throw SyntaxError("Expected ']'", it, end);
        temp = MemberExpression(std::move(temp), std::move(expression));
    }
    else if (is_match(it, end, "."))
    {
        auto expression = Expression{};
        if (!is_identifier_name(it, end, expression))
            throw SyntaxError("Expected <IdentifierName>", it, end);
        temp = MemberExpression(std::move(temp), std::move(expression));
    }
    result = std::move(temp);
    return true;
}

auto is_postfix_expression(auto &&it, auto end, auto &result)
{
    auto temp = Expression{};
    constexpr std::array operators{"++", "--"};
    if (!is_left_hand_side_expression(it, end, temp))
        return false;
    std::string op;
    if (!lookahead(it, end, &Token::separatedWithLineTerminator) && is_oneof(it, end, operators, op))
        temp = PostfixExpression(std::move(op), std::move(temp));
    result = std::move(temp);
    return true;
}

auto is_unary_expression(auto &&it, auto end, auto &result)
{
    constexpr std::array operators{"delete", "void", "typeof", "++", "--", "+", "-", "~", "!"};
    std::string op;
    if (is_oneof(it, end, operators, op))
    {
        auto temp = UnaryExpression{std::move(op)};
        if (!is_unary_expression(it, end, temp.rhs))
            throw SyntaxError("Expected <UnaryExpression>", it, end);
        result = std::move(temp);
        return true;
    }
    return is_postfix_expression(it, end, result);
}

template <typename T> auto is_binary_expression(auto &&it, auto end, auto &&operators, auto &&is_leaf, auto &result)
{
    auto temp = Expression{};
    if (!is_leaf(it, end, temp))
        return false;

    while (it != end)
    {
        if (std::find(operators.begin(), operators.end(), *it) == operators.end())
            break;
        std::string op = *it++;
        auto rhs = Expression{};
        if (!is_leaf(it, end, rhs))
            throw SyntaxError("Expected <...>", it, end);
        temp = T{std::move(op), std::move(temp), std::move(rhs)};
    }
    // for (std::string op; is_oneof(it, end, operators, op);)
    // {
    //     auto rhs = Expression{};
    //     if (!is_leaf(it, end, rhs))
    //         throw SyntaxError("Expected <...>", it, end);
    //     temp = T{std::move(op), std::move(temp), std::move(rhs)};
    // }
    result = std::move(temp);
    return true;
}

auto is_multiplicative_expression(auto &&it, auto end, auto &result)
{
    constexpr std::array operators{"*", "/", "%"};
    return is_binary_expression<MultiplicativeExpression>(it, end, operators, is_unary_expression, result);
}

auto is_additive_expression(auto &&it, auto end, auto &result)
{
    constexpr std::array operators{"+", "-"};
    return is_binary_expression<AdditiveExpression>(it, end, operators, is_multiplicative_expression, result);
}

auto is_shift_expression(auto &&it, auto end, auto &result)
{
    constexpr std::array operators{"<<", ">>", "<<<"};
    return is_binary_expression<ShiftExpression>(it, end, operators, is_additive_expression, result);
}

auto is_relational_expression(auto &&it, auto end, auto &result)
{
    constexpr std::array operators{"<", ">", "<=", ">=", "instanceof"};
    return is_binary_expression<RelationalExpression>(it, end, operators, is_shift_expression, result);
}

auto is_equality_expression(auto &&it, auto end, auto &result)
{
    constexpr std::array operators{"==", "!=", "===", "!=="};
    return is_binary_expression<EqualityExpression>(it, end, operators, is_relational_expression, result);
}

auto is_bitwise_and_expression(auto &&it, auto end, auto &result)
{
    constexpr std::array operators{"&"};
    return is_binary_expression<BitwiseANDExpression>(it, end, operators, is_relational_expression, result);
}

auto is_bitwise_xor_expression(auto &&it, auto end, auto &result)
{
    constexpr std::array operators{"^"};
    return is_binary_expression<BitwiseXORExpression>(it, end, operators, is_bitwise_and_expression, result);
}

auto is_bitwise_or_expression(auto &&it, auto end, auto &result)
{
    constexpr std::array operators{"|"};
    return is_binary_expression<BitwiseORExpression>(it, end, operators, is_bitwise_xor_expression, result);
}

auto is_logical_and_expression(auto &&it, auto end, auto &result)
{
    constexpr std::array operators{"&&"};
    return is_binary_expression<LogicalANDExpression>(it, end, operators, is_bitwise_or_expression, result);
}

auto is_logical_or_expression(auto &&it, auto end, auto &result)
{
    constexpr std::array operators{"||"};
    return is_binary_expression<LogicalORExpression>(it, end, operators, is_logical_and_expression, result);
}

auto is_conditional_expression(auto &&it, auto end, auto &result)
{
    return is_logical_or_expression(it, end, result);
}

auto is_assignment_expression(auto &&it, auto end, auto &result)
{
    return is_conditional_expression(it, end, result);
    // constexpr static std::vector<Token> operators = { "=", "*=", "/=", "%=", "+=", "-=", "<<=", ">>=", ">>>=", "&=",
    // "^=", "|=" }; return is_binary_expression<AssignmentExpression>(it, end, operators, is_conditional_expression,
    // result);
}

auto is_expression(auto &&it, auto end, auto &result)
{
    return false;
    // return is_assignment_expression(it, end, result);
}

auto is_block_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "{"))
        return false;
    auto temp = BlockStatement{};
    for (auto stmt = Statement{}; is_statement(it, end, stmt);)
        temp.statements.emplace_back(std::move(stmt));
    if (!is_match(it, end, "}"))
        throw SyntaxError("Expected '}'", it, end);
    result = std::move(temp);
    return true;
}

auto is_variable_declaration(auto &&it, auto end, auto &result)
{
    auto temp = VariableDeclaration{};
    auto identifier = Identifier{};
    if (!is_identifier(it, end, identifier))
        return false;
    // if (is_match(it, end, "=") && !is_expression(it, end, temp.initialiser))
    //     throw SyntaxError("Expected <AssignmentExpression>", it, end);
    // result.push_back(std::move(temp));
    return true;
}

auto is_variable_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "var"))
        return false;
    auto temp = VariableStatement{};
    while (is_variable_declaration(it, end, temp.variableDeclarations) && is_match(it, end, ","))
        ;
    if (!is_semicolon(it, end))
        throw new SyntaxError("Expected ';'", it, end);
    result = std::move(temp);
    return true;
}

auto is_empty_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, ";"))
        return false;
    result = EmptyStatement{};
    return true;
}

auto is_expression_statement(auto &&it, auto end, auto &result)
{
    if (lookahead(it, end, "{") || lookahead(it, end, "function"))
        return false;
    // auto temp = ExpressionStatement{};
    // if (!is_expression(it, end, temp.expression))
    //     return false;
    // if (!is_semicolon(it, end))
    //     throw SyntaxError("Expected ';'", it, end);
    // result = std::move(temp);
    return true;
}

auto is_if_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "if"))
        return false;
    auto temp = IfStatement{};
    if (!is_match(it, end, "("))
        throw SyntaxError("Expected '('", it, end);
    if (!is_expression(it, end, temp.expression))
        throw SyntaxError("Expected <Expression>", it, end);
    if (!is_match(it, end, ")"))
        throw SyntaxError("Expected ')'", it, end);
    if (!is_statement(it, end, temp.ifStatement))
        throw SyntaxError("Expected <Statement>", it, end);
    if (is_match(it, end, "else") && !is_statement(it, end, temp.elseStatement))
        throw SyntaxError("Expected <Statement>", it, end);
    result = std::move(temp);
    return true;
}

auto is_do_iteration_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "do"))
        return false;
    auto temp = IterationStatement{};
    if (!is_statement(it, end, temp.statement))
        throw new SyntaxError("Expected <Statement>", it, end);
    if (!is_match(it, end, "while"))
        throw new SyntaxError("Expected 'while'", it, end);
    if (!is_match(it, end, "("))
        throw new SyntaxError("Expected '('", it, end);
    if (!is_expression(it, end, temp.postExpression))
        throw new SyntaxError("Expected <Expression>", it, end);
    if (!is_match(it, end, ")"))
        throw new SyntaxError("Expected ')'", it, end);
    if (!is_semicolon(it, end))
        throw new SyntaxError("Expected ';'", it, end);
    result = std::move(temp);
    return true;
}

auto is_while_iteration_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "while"))
        return false;
    auto temp = IterationStatement{};
    if (!is_match(it, end, "("))
        throw new SyntaxError("Expected '('", it, end);
    if (!is_expression(it, end, temp.preExpression))
        throw new SyntaxError("Expected <Expression>", it, end);
    if (!is_match(it, end, ")"))
        throw new SyntaxError("Expected ')'", it, end);
    if (!is_statement(it, end, temp.statement))
        throw new SyntaxError("Expected <Statement>", it, end);
    result = std::move(temp);
    return true;
}

auto is_for_iteration_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "for"))
        return false;
    auto temp = IterationStatement{};
    if (!is_match(it, end, "("))
        throw new SyntaxError("Expected '('", it, end);
    if (is_match(it, end, "var"))
    {
    }
    else if (is_expression(it, end, temp.initExpression))
    {
    }
    else
        throw SyntaxError("Expected <Expression> or 'var'", it, end);

    if (is_match(it, end, ";"))
    {
    }
    else if (is_match(it, end, "in"))
    {
    }
    else
        throw SyntaxError("Expected ';' or 'in'", it, end);

    if (!is_statement(it, end, temp.statement))
        throw new SyntaxError("Expected <Statement>", it, end);
    result = std::move(temp);

    return true;
}

auto is_iteration_statement(auto &&it, auto end, auto &result)
{
    return is_do_iteration_statement(it, end, result) || is_while_iteration_statement(it, end, result) ||
           is_for_iteration_statement(it, end, result);
}

auto is_continue_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "continue"))
        return false;
    auto temp = ContinueStatement{};
    if (!lookahead(it, end, &Token::separatedWithLineTerminator))
        is_identifier(it, end, temp.identifier);
    if (!is_semicolon(it, end))
        throw SyntaxError("Expected ';'", it, end);
    result = std::move(temp);
    return true;
}

auto is_break_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "break"))
        return false;
    auto temp = BreakStatement{};
    if (!lookahead(it, end, &Token::separatedWithLineTerminator))
        is_identifier(it, end, temp.identifier);
    if (!is_semicolon(it, end))
        throw SyntaxError("Expected ';'", it, end);
    result = std::move(temp);
    return true;
}

auto is_return_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "return"))
        return false;
    auto temp = ReturnStatement{};
    if (!lookahead(it, end, &Token::separatedWithLineTerminator))
        is_identifier(it, end, temp.expression);
    if (!is_semicolon(it, end))
        throw SyntaxError("Expected ';'", it, end);
    result = std::move(temp);
    return true;
}

auto is_with_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "with"))
        return false;
    auto temp = WithStatement{};
    if (!is_match(it, end, "("))
        throw SyntaxError("Expected '('", it, end);
    if (!is_expression(it, end, temp.expression))
        throw SyntaxError("Expected <Expression>", it, end);
    if (!is_match(it, end, ")"))
        throw SyntaxError("Expected ')'", it, end);
    if (!is_statement(it, end, temp.statement))
        throw SyntaxError("Expected <Statement>", it, end);
    result = std::move(temp);
    return false;
}

auto is_labelled_statement(auto &&it, auto end, auto &result)
{
    if (!lookahead(it, end, &Token::isIdentifier, ":"))
        return false;
    auto temp = LabelledStatement{};
    is_identifier(it, end, temp.identifier);
    ++it;
    if (!is_statement(it, end, temp.statement))
        throw new SyntaxError("Expected <Statement>", it, end);
    result = std::move(temp);
    return true;
}

auto is_switch_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "switch"))
        return false;
    auto temp = SwitchStatement{};
    if (!is_match(it, end, "("))
        throw SyntaxError("Expected '('", it, end);
    if (!is_expression(it, end, temp.expression))
        throw SyntaxError("Expected <Expression>", it, end);
    if (!is_match(it, end, ")"))
        throw SyntaxError("Expected ')'", it, end);
    if (!is_match(it, end, "{"))
        throw SyntaxError("Expected '{'", it, end);

    while (true)
    {
        if (is_match(it, end, "case"))
        {
            auto clause = CaseClause{};
            if (!is_expression(it, end, clause.expression))
                throw SyntaxError("Expected <Expression>", it, end);
            if (!is_match(it, end, ":"))
                throw SyntaxError("Expected ':'", it, end);
            for (auto stmt = Statement{}; is_statement(it, end, stmt);)
                clause.statements.emplace_back(std::move(stmt));
            temp.caseClauses.emplace_back(std::move(clause));
        }
        else if (is_match(it, end, "default"))
        {
            if (!is_match(it, end, ":"))
                throw SyntaxError("Expected ':'", it, end);
            auto clause = DefaultClause{};
            for (auto stmt = Statement{}; is_statement(it, end, stmt);)
                clause.statements.emplace_back(std::move(stmt));
            temp.defaultClause = std::move(clause);
        }
        else
            break;
    }

    if (!is_match(it, end, "}"))
        throw SyntaxError("Expected '}'", it, end);

    result = std::move(temp);
    return true;
}

auto is_throw_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "throw"))
        return false;
    auto temp = ThrowStatement{};
    if (!is_line_terminator(it, end) || !is_expression(it, end, temp.expression))
        throw SyntaxError("Expected <Expression>", it, end);
    if (!is_semicolon(it, end))
        throw SyntaxError("Expected ';'", it, end);
    result = std::move(temp);
    return true;
}

auto is_try_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "try"))
        return false;
    auto temp = TryStatement{};
    if (!is_block_statement(it, end, temp.tryBlock))
        throw SyntaxError("Expected <Block>", it, end);
    if (is_match(it, end, "catch"))
    {
        if (!is_match(it, end, "("))
            throw SyntaxError("Expected '('", it, end);
        if (!is_identifier(it, end, temp.catchIdentifier))
            throw SyntaxError("Expected <Identifier>", it, end);
        if (!is_match(it, end, ")"))
            throw SyntaxError("Expected ')'", it, end);
        if (!is_block_statement(it, end, temp.catchBlock))
            throw SyntaxError("Expected <Block>", it, end);
        if (is_match(it, end, "finally") && !is_block_statement(it, end, temp.finallyBlock))
            throw SyntaxError("Expected <Block>", it, end);
    }
    else if (is_match(it, end, "finally") && !is_block(it, end, temp.finallyBlock))
        throw SyntaxError("Expected <Block>", it, end);
    else
        throw SyntaxError("Expected 'catch' or 'finally'", it, end);
    result = temp;
    return true;
}

auto is_debugger_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "debugger"))
        return false;
    if (!is_semicolon(it, end))
        throw SyntaxError("Expected ';'", it, end);
    result = DebuggerStatement{};
    return true;
}

auto is_statement(auto &&it, auto end, auto &result) -> bool
{
    return is_block_statement(it, end, result) || // is_variable_statement(it, end, result) ||
           is_empty_statement(it, end, result)    /* || is_expression_statement(it, end, result) ||
               is_if_statement(it, end, result) || is_iteration_statement(it, end, result) ||
               is_continue_statement(it, end, result) || is_break_statement(it, end, result) ||
               is_return_statement(it, end, result) || is_with_statement(it, end, result) ||
               is_switch_statement(it, end, result) || is_labelled_statement(it, end, result) ||
               is_throw_statement(it, end, result) || is_try_statement(it, end, result) ||
               is_debugger_statement(it, end, result)*/
        ;
}

} // namespace ecmascript
#endif