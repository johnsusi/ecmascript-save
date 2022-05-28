#include <catch2/catch_all.hpp>
#include <ecmascript/parser/statement.hpp>

TEST_CASE("assign", "[parser]")
{

    auto s = Statement {};
    REQUIRE(s.is_empty());
    s = EmptyStatement {};
    REQUIRE(s.is_a<EmptyStatement>());

    s = BlockStatement {};
    REQUIRE(s.is_a<BlockStatement>());

    s = VariableStatement {};
    REQUIRE(s.is_a<VariableStatement>());
    s = ExpressionStatement {};
    REQUIRE(s.is_a<ExpressionStatement>());
    s = IfStatement {};
    REQUIRE(s.is_a<IfStatement>());
    s = IterationStatement {};
    REQUIRE(s.is_a<IterationStatement>());
    s = ContinueStatement {};
    REQUIRE(s.is_a<ContinueStatement>());
    s = BreakStatement {};
    REQUIRE(s.is_a<BreakStatement>());
    s = ReturnStatement {};
    REQUIRE(s.is_a<ReturnStatement>());
    s = WithStatement {};
    REQUIRE(s.is_a<WithStatement>());
    s = LabelledStatement {};
    REQUIRE(s.is_a<LabelledStatement>());
    s = SwitchStatement {};
    REQUIRE(s.is_a<SwitchStatement>());
    s = ThrowStatement {};
    REQUIRE(s.is_a<ThrowStatement>());
    s = TryStatement {};
    REQUIRE(s.is_a<TryStatement>());
    s = DebuggerStatement {};
    REQUIRE(s.is_a<DebuggerStatement>());

}
