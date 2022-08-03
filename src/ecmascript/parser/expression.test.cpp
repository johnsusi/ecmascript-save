#include "expression.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace ecmascript;

TEST_CASE("ThisExpression", "[parser]")
{
    REQUIRE(ThisExpression{} == ThisExpression{});
}

TEST_CASE("Expressions", "[parser]")
{
    auto x = Expression{ThisExpression{}};
    auto y = ExpressionRef{std::move(x)};
    auto z = ExpressionRef{Expression{ThisExpression{}}};

    REQUIRE(y == z);
}