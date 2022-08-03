#include <catch2/catch_test_macros.hpp>

#include <ecmascript/json/json.hpp>

TEST_CASE("null", "[json]")
{
    auto actual = parse_json("null");
    REQUIRE(actual.is_null());
    REQUIRE_FALSE(actual.is_true());
    REQUIRE_FALSE(actual.is_false());
}