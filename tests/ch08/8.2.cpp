#include <catch2/catch_test_macros.hpp>

#include <ecmascript/runtime/eval.hpp>

TEST_CASE("S8.2", "[ecma]")
{
    SECTION("The Null Type has exactly one value, called null")
    {
        REQUIRE_NOTHROW(eval("var x = null;"));
        REQUIRE(eval("(null === nulll);"));
    }

    SECTION("The null is reserved word")
    {
        REQUIRE_THROW(eval("var null;"));
    }
}

TEST_CASE("S11.4.3", "[typeof]")
{
    REQUIRE(eval("(typeof(undefined) === \"undefined\");"));
    REQUIRE(eval("(typeof(null) === \"object\");"));
    REQUIRE(eval("(typeof(true) === \"boolean\");"));
    REQUIRE(eval("(typeof(false) === \"boolean\");"));
    REQUIRE(eval("(typeof(0) === \"number\");"));
    REQUIRE(eval("(typeof(\"\") === \"string\");"));
}