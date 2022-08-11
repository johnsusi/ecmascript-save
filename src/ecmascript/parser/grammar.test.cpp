#include "grammar.hpp"

#include <catch2/catch_test_macros.hpp>

#include <vector>

using namespace ecmascript;

TEST_CASE("Literal", "[parser]")
{
    SECTION("Null")
    {
        auto expected = Expression{LiteralExpression{NullLiteral{}}};
        auto tokens = {
            Token("null"),
        };
        auto actual = Expression{};
        bool ok = is_literal(tokens.begin(), tokens.end(), actual);
        REQUIRE(ok);
        REQUIRE(expected == actual);
    }
    SECTION("True")
    {
        auto expected = Expression{LiteralExpression{BooleanLiteral{true}}};
        auto tokens = {
            Token("true"),
        };
        auto actual = Expression{};
        bool ok = is_literal(tokens.begin(), tokens.end(), actual);
        REQUIRE(ok);
        REQUIRE(expected == actual);
    }
    SECTION("False")
    {
        auto expected = Expression{LiteralExpression{BooleanLiteral{false}}};
        auto tokens = {
            Token("false"),
        };
        auto actual = Expression{};
        bool ok = is_literal(tokens.begin(), tokens.end(), actual);
        REQUIRE(ok);
        REQUIRE(expected == actual);
    }
}

TEST_CASE("BlockStatement", "[parser]")
{

    SECTION("Empty Block")
    {
        auto expected = Statement{
            BlockStatement{},
        };
        auto tokens = {
            Token("{"),
            Token("}"),
        };
        auto actual = Statement{};
        bool ok = is_block_statement(tokens.begin(), tokens.end(), actual);
        REQUIRE(ok);
        REQUIRE(expected == actual);
    }

    SECTION("Block With Statements")
    {
        auto expected = Statement{
            BlockStatement{
                StatementList{
                    EmptyStatement{},
                    EmptyStatement{},
                    EmptyStatement{},
                },
            },

        };
        auto tokens = {
            Token("{"), Token(";"), Token(";"), Token(";"), Token("}"),
        };
        auto actual = Statement{};
        bool ok = is_block_statement(tokens.begin(), tokens.end(), actual);
        REQUIRE(ok);
        REQUIRE(expected == actual);
    }
}

TEST_CASE("VariableStatement", "[parser]")
{
    auto expected = Statement{
        VariableStatement{},
    };
    auto tokens = {
        Token("var"),
        Token("x", Token::IdentifierName{}),
        Token(";"),
    };
    auto actual = Statement{};
    bool ok = is_variable_statement(tokens.begin(), tokens.end(), actual);
    REQUIRE(ok);
    REQUIRE(expected == actual);
}

TEST_CASE("EmptyStatement", "[parser]")
{
    auto expected = Statement{
        EmptyStatement{},
    };
    auto tokens = {
        Token(";"),
    };
    auto actual = Statement{};
    bool ok = is_empty_statement(tokens.begin(), tokens.end(), actual);
    REQUIRE(ok);
    REQUIRE(expected == actual);
}

TEST_CASE("ExpressionStatement", "[parser]")
{
    auto expected = Statement{
        ExpressionStatement{
            ThisExpression{},
        },
    };
    auto tokens = {
        Token("this"),
        Token(";"),
    };
    auto actual = Statement{};
    bool ok = is_expression_statement(tokens.begin(), tokens.end(), actual);
    REQUIRE(ok);
    REQUIRE(expected == actual);
}

TEST_CASE("IfStatement", "[parser]")
{
    SECTION("If")
    {
        auto expected = Statement{
            IfStatement{
                ThisExpression{},
                EmptyStatement{},
            },
        };
        auto tokens = {
            Token("if"), Token("("), Token("this"), Token(")"), Token(";"),
        };
        auto actual = Statement{};
        bool ok = is_if_statement(tokens.begin(), tokens.end(), actual);
        REQUIRE(ok);
        REQUIRE(expected == actual);
    }
    SECTION("If-Else")
    {

        auto expected = Statement{
            IfStatement{ThisExpression{}, EmptyStatement{}, EmptyStatement{}},
        };
        auto tokens = {
            Token("if"), Token("("), Token("this"), Token(")"), Token(";"), Token("else"), Token(";"),
        };
        auto actual = Statement{};
        bool ok = is_if_statement(tokens.begin(), tokens.end(), actual);
        REQUIRE(ok);
        REQUIRE(expected == actual);
    }
}

TEST_CASE("IterationStatement", "[parser]")
{
    SECTION("Do-While")
    {
        auto expected = Statement{
            IterationStatement{
                .postTestExpression = LiteralExpression{BooleanLiteral{true}},
                .statement = EmptyStatement{},
            },
        };
        auto tokens = {
            Token("do"), Token(";"), Token("while"), Token("("), Token("true"), Token(")"),
        };
        auto actual = Statement{};
        bool ok = is_iteration_statement(tokens.begin(), tokens.end(), actual);
        REQUIRE(ok);
        REQUIRE(expected == actual);
    }

    SECTION("While")
    {
        auto expected = Statement{
            IterationStatement{
                .preTestExpression = LiteralExpression{BooleanLiteral{true}},
                .statement = EmptyStatement{},
            },
        };
        auto tokens = {Token("while"), Token("("), Token("true"), Token(")"), Token(";")};
        auto actual = Statement{};
        bool ok = is_iteration_statement(tokens.begin(), tokens.end(), actual);
        REQUIRE(ok);
        REQUIRE(expected == actual);
    }
}

TEST_CASE("ContinueStatement", "[parser]")
{
    SECTION("Continue")
    {
        auto expected = Statement{
            ContinueStatement{},
        };
        auto tokens = {
            Token("continue"),
            Token(";"),
        };
        auto actual = Statement{};
        bool ok = is_continue_statement(tokens.begin(), tokens.end(), actual);
        REQUIRE(ok);
        REQUIRE(expected == actual);
    }

    SECTION("Continue-Identifier")
    {
        auto expected = Statement{
            ContinueStatement{
                .identifier = {"x"},
            },
        };
        auto tokens = {
            Token("continue"),
            Token("x"),
            Token(";"),
        };
        auto actual = Statement{};
        bool ok = is_continue_statement(tokens.begin(), tokens.end(), actual);
        REQUIRE(ok);
        REQUIRE(expected == actual);
    }
}

TEST_CASE("BreakStatement", "[parser]")
{
    SECTION("Break")
    {
        auto expected = Statement{
            BreakStatement{},
        };
        auto tokens = {
            Token("break"),
            Token(";"),
        };
        auto actual = Statement{};
        bool ok = is_break_statement(tokens.begin(), tokens.end(), actual);
        REQUIRE(ok);
        REQUIRE(expected == actual);
    }

    SECTION("Break-Identifier")
    {
        auto expected = Statement{
            BreakStatement{
                .identifier = {"x"},
            },
        };
        auto tokens = {
            Token("break"),
            Token("x"),
            Token(";"),
        };
        auto actual = Statement{};
        bool ok = is_break_statement(tokens.begin(), tokens.end(), actual);
        REQUIRE(ok);
        REQUIRE(expected == actual);
    }
}

TEST_CASE("ReturnStatement", "[parser]")
{
    SECTION("Return")
    {
        auto expected = Statement{
            ReturnStatement{},
        };
        auto tokens = {
            Token("return"),
            Token(";"),
        };
        auto actual = Statement{};
        bool ok = is_return_statement(tokens.begin(), tokens.end(), actual);
        REQUIRE(ok);
        REQUIRE(expected == actual);
    }

    SECTION("Return-Expression")
    {
        auto expected = Statement{
            ReturnStatement{
                .expression =
                    LiteralExpression{
                        NullLiteral{},
                    },
            },
        };
        auto tokens = {
            Token("return"),
            Token("null"),
            Token(";"),
        };
        auto actual = Statement{};
        bool ok = is_return_statement(tokens.begin(), tokens.end(), actual);
        REQUIRE(ok);
        REQUIRE(expected == actual);
    }
}

TEST_CASE("WithStatement", "[parser]")
{

    auto expected = Statement{
        WithStatement{
            .expression = ThisExpression{},
            .statement = EmptyStatement{},
        },
    };
    auto tokens = {
        Token("with"), Token("("), Token("this"), Token(")"), Token(";"),
    };
    auto actual = Statement{};
    bool ok = is_with_statement(tokens.begin(), tokens.end(), actual);
    REQUIRE(ok);
    REQUIRE(expected == actual);
}

TEST_CASE("SwitchStatement", "[parser]")
{

    auto expected = Statement{
        SwitchStatement{
            .expression = IdentifierExpression{"x"},
            .caseClauses =
                {
                    CaseClause{
                        .expression = LiteralExpression{NumericLiteral{5.0}},
                        .statements =
                            StatementList{
                                EmptyStatement{},
                            },
                    },
                },
        },
    };
    auto tokens = {
        Token("switch"), Token("("), Token("x"), Token(")"), Token("{"),
        Token("case"),   Token(5),   Token(":"), Token(";"), Token("}"),
    };
    auto actual = Statement{};
    bool ok = is_switch_statement(tokens.begin(), tokens.end(), actual);
    REQUIRE(ok);
    REQUIRE(expected == actual);
}