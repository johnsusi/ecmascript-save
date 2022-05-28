#include <catch2/catch_all.hpp>
#include <ecmascript/json/json.hpp>
#include <string>

TEST_CASE("whitespace", "[json]")
{
    REQUIRE(parse_json(R"(
        {
            "foo": "bar",
            "baz": [
                { "foo": "bar" },
                { "foo":
                  "bar" }
                ,
                [1,
                  2,   3 ]

            ]
        }
    )") == R"({"foo":"bar","baz":[{"foo":"bar"},{"foo":"bar"},[1,2,3]]})");
}
