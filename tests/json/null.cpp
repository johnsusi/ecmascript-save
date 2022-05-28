#include <catch2/catch_all.hpp>
#include <ecmascript/json/json.hpp>
#include <string>

TEST_CASE("null", "[json]")
{
    auto source = GENERATE(as<std::string>{}, "null");
    auto json = parse_json(source);
    REQUIRE(json.is_null());
    REQUIRE(json.to_string() == source);
}