#include <catch2/catch_test_macros.hpp>

#include <ecmascript/runtime/eval.hpp>

TEST_CASE("S8.1_A1_T1", "[ecma]")
{
    REQUIRE(eval("var x = undefined").is_undefined());
}

TEST_CASE("S8.1_A1_T2", "[ecma]")
{
    REQUIRE(eval("(typeof(undefined) === \"undefined\")"));
    REQUIRE(eval("(typeof(void 0) === \"undefined\")"));
    REQUIRE(eval("(undefined === void 0)"));
}

TEST_CASE("S8.1_A2_T1", "[ecma]")
{
    REQUIRE(eval("var x; (x === undefined)"));
    REQUIRE(eval("var x; (typeof(x) === \"undefined\")"));
    REQUIRE(eval("var x; (x === void 0)"));
}

TEST_CASE("S8.1_A2_T2", "[ecma]")
{
    REQUIRE(eval(R"""
        function test1(x) {
        return x;
        }
    
        (test1() === void 0);
    """));
    REQUIRE(eval(R"""
        function test2() {
        }
    
        (test2() === void 0);
    """));
}

TEST_CASE("S8.1_A3", "[ecma]")
{
    REQUIRE(eval("var undefined = 1;") == 1);
}

TEST_CASE("S8.1_A4", "[ecma]")
{
    REQUIRE(eval("(new Object()).newProperty !== undefined)"));
}