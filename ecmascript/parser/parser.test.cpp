#include "parser.cpp"

#define CONFIG_CATCH_MAIN
#include <catch2/catch_all.hpp>

// TEST_CASE("is_block_statement", "[parser]")
// {
//     auto tokens = { Token("{"), Token("}") };
//     auto it = tokens.begin();
//     auto result = Statement {};
//     REQUIRE(parse_block_statement(it, tokens.end(), result));
//     REQUIRE(std::holds_alternative<BlockStatement>(result));
//     REQUIRE(it == tokens.end());
// }

// TEST_CASE("is_variable_statement", "[parser]")
// {
//     auto tokens = { Token("var"), Token(";") };
//     auto it = tokens.begin();
//     auto result = Statement {};
//     REQUIRE(parse_variable_statement(it, tokens.end(), result));
//     REQUIRE(std::holds_alternative<VariableStatement>(result));
//     REQUIRE(it == tokens.end());
// }

// TEST_CASE("is_empty_statement", "[parser]")
// {
//     auto tokens = { Token(";")};
//     auto it = tokens.begin();
//     auto result = Statement {};
//     REQUIRE(parse_empty_statement(it, tokens.end(), result));
//     REQUIRE(std::holds_alternative<EmptyStatement>(result));
//     REQUIRE(it == tokens.end());
// }

// TEST_CASE("is_try_statement", "[parser]")
// {
//     auto tokens = {Token("try"), Token("{"), Token("}"), Token("finally"), Token("{"), Token("}")};
//     auto it = tokens.begin();
//     auto result = Statement {};
//     REQUIRE(parse_try_statement(it, tokens.end(), result));
//     REQUIRE(std::holds_alternative<TryStatement>(result));
//     REQUIRE(it == tokens.end());
// }

// TEST_CASE("is_debugger_statement", "[parser]")
// {
//     auto tokens = {Token("debugger"), Token(";")};
//     auto it = tokens.begin();
//     auto result = Statement {};
//     REQUIRE(parse_debugger_statement(it, tokens.end(), result));
//     REQUIRE(std::holds_alternative<DebuggerStatement>(result));
//     REQUIRE(it == tokens.end());
// }