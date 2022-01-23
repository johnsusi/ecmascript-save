#include <catch2/catch.hpp>
#include <ecmascript/json/json.hpp>
#include <string>

TEST_CASE("string", "[json]")
{
    auto source = GENERATE(as<std::string>{}, "\"\"", "\"\\\"\"", "\"abcdef\"");
    auto json = parse_json(source);
    REQUIRE(json.is_string());
    REQUIRE(json.to_string() == source);
}