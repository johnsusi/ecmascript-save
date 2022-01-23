#include "json.cpp"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

// TEST_CASE("is_sign", "[json]")
// {
//     auto source = GENERATE(as<std::string>{}, "+", "-", "" );
//     REQUIRE(is_sign(source.begin(), source.end()));
// }

// TEST_CASE("is_exponent", "[json]")
// {
//     auto source = GENERATE(as<std::string>{}, "e1", "E1", "e-1", "E-1", "e+1", "E+1", "e-1234567890123456789", "");
//     REQUIRE(is_exponent(source.begin(), source.end()));
// }

// TEST_CASE("is_fraction", "[json]")
// {
//     auto source = GENERATE(as<std::string>{}, ".0", ".0123456789", "");
//     REQUIRE(is_exponent(source.begin(), source.end()));
// }

// TEST_CASE("is_onenine", "[json]")
// {
//     auto source = GENERATE(as<std::string>{}, "1", "2", "3", "4", "5", "6", "7", "8", "9" );
//     REQUIRE(is_onenine(source.begin(), source.end()));
// }

// TEST_CASE("is_digit", "[json]")
// {
//     auto source = GENERATE(as<std::string>{}, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" );
//     REQUIRE(is_digit(source.begin(), source.end()));
// }
