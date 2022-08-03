#include "grammar.hpp"

#include <catch2/catch_test_macros.hpp>

#include <vector>

using namespace ecmascript;

TEST_CASE("Block Statement", "[parser]")
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
            BlockStatement{.statements =
                               {
                                   Statement(EmptyStatement{}),
                                   Statement(EmptyStatement{}),
                                   Statement(EmptyStatement{}),
                               }},

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

TEST_CASE("Variable Statement", "[parser]")
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

TEST_CASE("Empty Statement", "[parser]")
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

// TEST_CASE("Expression Statement", "[parser]")
// {
//     auto expected = Statement{
//         ExpressionStatement{
//             Expression{
//                 ThisExpression{},
//             },
//         },
//     };
//     auto tokens = {
//         Token("this"),
//         Token(";"),
//     };
//     auto actual = Statement{};
//     bool ok = is_expression_statement(tokens.begin(), tokens.end(), actual);
//     REQUIRE(ok);
//     REQUIRE(expected == actual);
// }
