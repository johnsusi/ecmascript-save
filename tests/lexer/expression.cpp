#include <catch2/catch.hpp>
#include <ecmascript/lexer/lexer.hpp>
#include <string>

TEST_CASE("expression", "[lexer]")
{
    auto source = GENERATE(as<std::string>{}, "1+2*3");

    auto lexer = Lexer { source };

    // auto it = lexer.begin();
    // auto token = *it++;
    // REQUIRE(token.is_numeric_literal());
    // token = *it++;
    // REQUIRE(token.is_punctuator());
    // token = *it++;
    // REQUIRE(token.is_numeric_literal());
    // token = *it++;
    // REQUIRE(token.is_punctuator());
    // token = *it++;
    // REQUIRE(token.is_numeric_literal());
    // token = *it++;
    // REQUIRE(it == lexer.end());

}