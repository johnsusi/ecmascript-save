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

bool is_match(auto &&it, auto end, auto &&what)
{
    if (it == end || *it != what)
        return false;
    return ++it, true;
}

bool is_match(auto &&it, auto end, auto &&what, auto &result)
{
    if (it == end || *it != what)
        return false;
    result = *it;
    return ++it, true;
}

template <typename T> bool is_match(auto &&it, auto end, std::initializer_list<T> what, auto &result)
{
    if (it == end || std::find(what.begin(), what.end(), *it) == what.end())
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

bool is_match(auto &&it, auto end, bool (Token::*what)() const, auto &result)
{
    if (it == end || !(it->*what)())
        return false;
    result = *it;
    return ++it, true;
}

template <typename... Args> bool lookahead(auto it, auto end, Args &&...what)
{
    return (is_match(it, end, std::forward<Args>(what)) && ...);
}

bool is_line_terminator(auto it, auto end)
{
    return it != end && it->separatedWithLineTerminator();
}

bool is_semicolon(auto &&it, auto end)
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

bool is_this(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "this"))
        return false;
    result = ThisExpression{};
    return true;
}

bool is_identifier(auto &&it, auto end, Expression &result)
{
    std::string s;
    if (!is_match(it, end, &Token::isIdentifier, s))
        return false;
    result = IdentifierExpression{s};
    return true;
}

bool is_identifier(auto &&it, auto end, Identifier &result)
{
    return is_match(it, end, &Token::isIdentifier, result.value);
}

bool is_identifier_name(auto &&it, auto end, auto &result)
{
    std::string s;
    if (!is_match(it, end, &Token::isIdentifierName, s))
        return false;
    // result = Identifier{it->value};
    return true;
}

bool is_literal(auto &&it, auto end, auto &result)
{
    if (is_match(it, end, "null"))
        result = LiteralExpression{NullLiteral{}};
    else if (is_match(it, end, "false"))
        result = LiteralExpression{BooleanLiteral{false}};
    else if (is_match(it, end, "true"))
        result = LiteralExpression{BooleanLiteral{true}};
    else
        return false;
    return true;
}

bool is_array_literal(auto &&it, auto end, auto &result)
{
    return false;
}

bool is_object_literal(auto &&it, auto end, auto &result)
{
    return false;
}

bool is_grouping(auto &&it, auto end, auto &result)
{
    return false;
}

bool is_primary_expression(auto &&it, auto end, auto &result)
{
    return is_this(it, end, result) || is_identifier(it, end, result) || is_literal(it, end, result) ||
           is_array_literal(it, end, result) || is_object_literal(it, end, result) || is_grouping(it, end, result);
}

bool is_arguments(auto &&it, auto end, auto &result)
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
    return true;
}

bool is_member_expression(auto &&it, auto end, auto &result)
{
    if (!is_primary_expression(it, end, result) /*|| !is_function_expression(it, end, temp)*/)
        return false;

    while (true)
    {
        if (is_match(it, end, "["))
        {
            auto temp = Expression{};
            if (!is_expression(it, end, temp))
                throw SyntaxError("Expected <Expression>", it, end);
            if (!is_match(it, end, "]"))
                throw SyntaxError("Expected ']", it, end);
            result = MemberExpression{std::move(result), std::move(temp)};
        }
        else if (is_match(it, end, "."))
        {
            auto identifier = Expression{};
            if (!is_identifier_name(it, end, identifier))
                throw SyntaxError("Expected <IdentifierName>", it, end);
            result = MemberExpression{std::move(result), std::move(identifier)};
        }
        else
            break;
    }
    return true;
}

bool is_new_expression(auto &&it, auto end, auto &result)
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

bool is_left_hand_side_expression(auto &&it, auto end, auto &result)
{
    if (!is_new_expression(it, end, result))
        return false;

    auto arguments = Arguments{};
    if (is_arguments(it, end, arguments))
    {
        // temp = CallExpression{std::move(temp), std::move(arguments)};
        return false;
    }
    else if (is_match(it, end, "["))
    {
        // auto expression = Expression{};
        // if (!is_expression(it, end, expression))
        //     throw SyntaxError("Expected <Expression>", it, end);
        // if (!is_match(it, end, "]"))
        //     throw SyntaxError("Expected ']'", it, end);
        // temp = MemberExpression{std::move(temp), std::move(expression)};
        return false;
    }
    else if (is_match(it, end, "."))
    {
        // auto expression = Expression{};
        // if (!is_identifier_name(it, end, expression))
        //     throw SyntaxError("Expected <IdentifierName>", it, end);
        // temp = MemberExpression{std::move(temp), std::move(expression)};
        return false;
    }
    // result = std::move(temp);
    return true;
}

bool is_postfix_expression(auto &&it, auto end, auto &result)
{
    if (!is_left_hand_side_expression(it, end, result))
        return false;
    if (std::string op; !lookahead(it, end, &Token::separatedWithLineTerminator) && is_match(it, end, {"++", "--"}, op))
        result = PostfixExpression{std::move(op), std::move(result)};
    return true;
}

bool is_unary_expression(auto &&it, auto end, auto &result)
{
    if (std::string op; is_match(it, end, {"delete", "void", "typeof", "++", "--", "+", "-", "~", "!"}, op))
    {
        auto temp = Expression{};
        if (!is_unary_expression(it, end, temp))
            throw SyntaxError("Expected <UnaryExpression>", it, end);
        result = UnaryExpression{std::move(op), std::move(temp)};
        return true;
    }

    return is_postfix_expression(it, end, result);
}

bool is_multiplicative_expression(auto &&it, auto end, auto &result)
{
    if (!is_unary_expression(it, end, result))
        return false;
    for (std::string op; is_match(it, end, {"*", "/", "%"}, op);)
    {
        auto temp = Expression{};
        if (!is_unary_expression(it, end, temp))
            throw SyntaxError("Expected <UnaryExpression>", it, end);
        result = MultiplicativeExpression{std::move(op), std::move(result), std::move(temp)};
    }
    return true;
}

bool is_additive_expression(auto &&it, auto end, auto &result)
{
    if (!is_multiplicative_expression(it, end, result))
        return false;
    for (std::string op; is_match(it, end, {"+", "-"}, op);)
    {
        auto temp = Expression{};
        if (!is_multiplicative_expression(it, end, temp))
            throw SyntaxError("Expected <MultiplicativeExpression>", it, end);
        result = AdditiveExpression{std::move(op), std::move(result), std::move(temp)};
    }
    return true;
}

bool is_shift_expression(auto &&it, auto end, auto &result)
{
    if (!is_additive_expression(it, end, result))
        return false;
    for (std::string op; is_match(it, end, {"<<", ">>", "<<<"}, op);)
    {
        auto temp = Expression{};
        if (!is_additive_expression(it, end, temp))
            throw SyntaxError("Expected <AdditiveExpression>", it, end);
        result = ShiftExpression{std::move(op), std::move(result), std::move(temp)};
    }
    return true;
}

bool is_relational_expression(auto &&it, auto end, auto &result)
{
    if (!is_shift_expression(it, end, result))
        return false;
    for (std::string op; is_match(it, end, {"<", ">", "<=", ">=", "instanceof"}, op);)
    {
        auto temp = Expression{};
        if (!is_shift_expression(it, end, temp))
            throw SyntaxError("Expected <ShiftExpression>", it, end);
        result = RelationalExpression{std::move(op), std::move(result), std::move(temp)};
    }
    return true;
}

bool is_equality_expression(auto &&it, auto end, auto &result)
{
    if (!is_relational_expression(it, end, result))
        return false;
    for (std::string op; is_match(it, end, {"==", "!=", "===", "!=="}, op);)
    {
        auto temp = Expression{};
        if (!is_relational_expression(it, end, temp))
            throw SyntaxError("Expected <RelationalExpression>", it, end);
        result = EqualityExpression{std::move(op), std::move(result), std::move(temp)};
    }
    return true;
}

bool is_bitwise_and_expression(auto &&it, auto end, auto &result)
{
    if (!is_equality_expression(it, end, result))
        return false;
    for (std::string op; is_match(it, end, "&", op);)
    {
        auto temp = Expression{};
        if (!is_equality_expression(it, end, temp))
            throw SyntaxError("Expected <EqualityExpression>", it, end);
        result = BitwiseANDExpression{std::move(op), std::move(result), std::move(temp)};
    }
    return true;
}

bool is_bitwise_xor_expression(auto &&it, auto end, auto &result)
{
    if (!is_bitwise_and_expression(it, end, result))
        return false;
    for (std::string op; is_match(it, end, "|", op);)
    {
        auto temp = Expression{};
        if (!is_bitwise_and_expression(it, end, temp))
            throw SyntaxError("Expected <BitwiseANDExpression>", it, end);
        result = BitwiseXORExpression{std::move(op), std::move(result), std::move(temp)};
    }
    return true;
}

bool is_bitwise_or_expression(auto &&it, auto end, auto &result)
{
    if (!is_bitwise_xor_expression(it, end, result))
        return false;
    for (std::string op; is_match(it, end, "|", op);)
    {
        auto temp = Expression{};
        if (!is_bitwise_xor_expression(it, end, temp))
            throw SyntaxError("Expected <BitwiseXORExpression>", it, end);
        result = BitwiseORExpression{std::move(op), std::move(result), std::move(temp)};
    }
    return true;
}

bool is_logical_and_expression(auto &&it, auto end, auto &result)
{
    if (!is_bitwise_or_expression(it, end, result))
        return false;
    for (std::string op; is_match(it, end, "&&", op);)
    {
        auto temp = Expression{};
        if (!is_bitwise_or_expression(it, end, temp))
            throw SyntaxError("Expected <BitwiseORExpression>", it, end);
        result = LogicalANDExpression{std::move(op), std::move(result), std::move(temp)};
    }
    return true;
}

bool is_logical_or_expression(auto &&it, auto end, auto &result)
{
    if (!is_logical_and_expression(it, end, result))
        return false;
    for (std::string op; is_match(it, end, "||", op);)
    {
        auto temp = Expression{};
        if (!is_logical_and_expression(it, end, temp))
            throw SyntaxError("Expected <LogicalAndExpression>", it, end);
        result = LogicalORExpression{std::move(op), std::move(result), std::move(temp)};
    }
    return true;
}

bool is_conditional_expression(auto &&it, auto end, auto &result)
{
    return is_logical_or_expression(it, end, result);
}

bool is_assignment_expression(auto &&it, auto end, auto &result)
{
    return is_conditional_expression(it, end, result);
    // constexpr static std::vector<Token> operators = { "=", "*=", "/=", "%=", "+=", "-=", "<<=", ">>=", ">>>=", "&=",
    // "^=", "|=" }; return is_binary_expression<AssignmentExpression>(it, end, operators, is_conditional_expression,
    // result);
}

bool is_expression(auto &&it, auto end, auto &result)
{
    return is_assignment_expression(it, end, result);
}

bool is_block_statement(auto &&it, auto end, auto &result)
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

bool is_variable_declaration(auto &&it, auto end, auto &result)
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

bool is_variable_statement(auto &&it, auto end, auto &result)
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

bool is_empty_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, ";"))
        return false;
    result = EmptyStatement{};
    return true;
}

bool is_expression_statement(auto &&it, auto end, auto &result)
{
    if (lookahead(it, end, "{") || lookahead(it, end, "function"))
        return false;
    auto temp = ExpressionStatement{};
    if (!is_expression(it, end, temp.expression))
        return false;
    if (!is_semicolon(it, end))
        throw SyntaxError("Expected ';'", it, end);
    result = std::move(temp);
    return true;
}

bool is_if_statement(auto &&it, auto end, auto &result)
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

bool is_do_iteration_statement(auto &&it, auto end, auto &result)
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
    if (!is_expression(it, end, temp.postTestExpression))
        throw new SyntaxError("Expected <Expression>", it, end);
    if (!is_match(it, end, ")"))
        throw new SyntaxError("Expected ')'", it, end);
    if (!is_semicolon(it, end))
        throw new SyntaxError("Expected ';'", it, end);
    result = std::move(temp);
    return true;
}

bool is_while_iteration_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "while"))
        return false;
    auto temp = IterationStatement{};
    if (!is_match(it, end, "("))
        throw new SyntaxError("Expected '('", it, end);
    if (!is_expression(it, end, temp.preTestExpression))
        throw new SyntaxError("Expected <Expression>", it, end);
    if (!is_match(it, end, ")"))
        throw new SyntaxError("Expected ')'", it, end);
    if (!is_statement(it, end, temp.statement))
        throw new SyntaxError("Expected <Statement>", it, end);
    result = std::move(temp);
    return true;
}

bool is_for_iteration_statement(auto &&it, auto end, auto &result)
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

bool is_iteration_statement(auto &&it, auto end, auto &result)
{
    return is_do_iteration_statement(it, end, result) || is_while_iteration_statement(it, end, result) ||
           is_for_iteration_statement(it, end, result);
}

bool is_continue_statement(auto &&it, auto end, auto &result)
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

bool is_break_statement(auto &&it, auto end, auto &result)
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

bool is_return_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "return"))
        return false;
    auto temp = ReturnStatement{};
    if (!lookahead(it, end, &Token::separatedWithLineTerminator))
        is_expression(it, end, temp.expression);
    if (!is_semicolon(it, end))
        throw SyntaxError("Expected ';'", it, end);
    result = std::move(temp);
    return true;
}

bool is_with_statement(auto &&it, auto end, auto &result)
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
    return true;
}

bool is_labelled_statement(auto &&it, auto end, auto &result)
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

bool is_switch_statement(auto &&it, auto end, auto &result)
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

bool is_throw_statement(auto &&it, auto end, auto &result)
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

bool is_try_statement(auto &&it, auto end, auto &result)
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

bool is_debugger_statement(auto &&it, auto end, auto &result)
{
    if (!is_match(it, end, "debugger"))
        return false;
    if (!is_semicolon(it, end))
        throw SyntaxError("Expected ';'", it, end);
    result = DebuggerStatement{};
    return true;
}

bool is_statement(auto &&it, auto end, auto &result)
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