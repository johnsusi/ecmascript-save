#include "lexer.cpp"

#include <catch2/catch_test_macros.hpp>

// TEST_CASE("false", "[json]")
// {
//     REQUIRE();
// }

TEST_CASE("Iterate over lexer", "[lexer]")
{

    // auto lexer = Lexer{Token{}};

    SECTION("using iterators")
    {
        // auto it = lexer.begin();
        // auto end = lexer.end();
        // REQUIRE(it != end);
        // REQUIRE(++it == end);
    }

    SECTION("using range-based for loop")
    {
        // for (auto &&token : lexer)
        //     ;
    }
}