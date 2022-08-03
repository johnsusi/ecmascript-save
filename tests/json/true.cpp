#include <catch2/catch_test_macros.hpp>

#include <ecmascript/json/json.hpp>

TEST_CASE("true", "[json]")
{
    auto actual = parse_json("true");
    REQUIRE(actual.is_true());
    REQUIRE_FALSE(actual.is_false());
    REQUIRE_FALSE(actual.is_null());
}