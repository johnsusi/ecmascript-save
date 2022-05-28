#include <catch2/catch_all.hpp>
#include <ecmascript/json/json.hpp>
#include <string>

TEST_CASE("false", "[json]")
{
    auto source = GENERATE(as<std::string>{}, "false");
    auto json = parse_json(source);
    REQUIRE(json.is_false());
    REQUIRE(json.to_string() == source);
}