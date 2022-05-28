#include <catch2/catch_all.hpp>
#include <ecmascript/json/json.hpp>
#include <string>

TEST_CASE("number", "[json]")
{
    auto source = GENERATE(as<std::string>{}, "0", "1", "-1", "1.23", "-1.23", "1.23e456", "1.23e+456", "1.23e-456",
                           "-1.23e-456", "1e0", "-1e-1");
    auto json = parse_json(source);
    REQUIRE(json.is_number());
    REQUIRE(json.to_string() == source);
}