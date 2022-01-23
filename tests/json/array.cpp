#include <catch2/catch.hpp>
#include <ecmascript/json/json.hpp>
#include <string>

TEST_CASE("array", "[json]")
{
    auto source = GENERATE(as<std::string>{}, "[]", "[null]", "[true,false]", "[-1,0,1,2,3,4,5,6]",
                           "[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]");
    auto json = parse_json(source);
    REQUIRE(json.is_array());
    REQUIRE(json.to_string() == source);
}
