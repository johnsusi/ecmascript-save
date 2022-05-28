#include <catch2/catch_all.hpp>
#include <ecmascript/json/json.hpp>
#include <string>

TEST_CASE("object", "[json]")
{
    auto source = GENERATE(as<std::string>{}, "{}", "{\"foo\":true}", "{\"bar\":\"foo\",\"foo\":\"bar\"}",
                           "{\"foo\":{\"foo\":{\"foo\":{\"foo\":true}}}}");
    auto json = parse_json(source);
    REQUIRE(json.is_object());
    REQUIRE(json.to_string() == source);
}