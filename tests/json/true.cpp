#include <catch2/catch.hpp>
#include <ecmascript/json/json.hpp>
#include <string>

TEST_CASE("true", "[json]")
{
    auto source = GENERATE(as<std::string>{}, "true");
    auto json = parse_json(source);
    REQUIRE(json.is_true());
    REQUIRE(json.to_string() == source);
}