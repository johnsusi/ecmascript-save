#include <catch2/catch_all.hpp>
#include <ecmascript/parser/statement.hpp>

TEST_CASE("assign", "[parser]")
{
    auto s = Statement {};
    REQUIRE(s.empty());
    s = EmptyStatement {};
    REQUIRE(s.holds<EmptyStatement>());
    s = BlockStatement {};
    REQUIRE(s.holds<BlockStatement>());
    s = VariableStatement {};
    REQUIRE(s.holds<VariableStatement>());
    s = ExpressionStatement {};
    REQUIRE(s.holds<ExpressionStatement>());
    s = IfStatement {};
    REQUIRE(s.holds<IfStatement>());
    s = IterationStatement {};
    REQUIRE(s.holds<IterationStatement>());
    s = ContinueStatement {};
    REQUIRE(s.holds<ContinueStatement>());
    s = BreakStatement {};
    REQUIRE(s.holds<BreakStatement>());
    s = ReturnStatement {};
    REQUIRE(s.holds<ReturnStatement>());
    s = WithStatement {};
    REQUIRE(s.holds<WithStatement>());
    s = LabelledStatement {};
    REQUIRE(s.holds<LabelledStatement>());
    s = SwitchStatement {};
    REQUIRE(s.holds<SwitchStatement>());
    s = ThrowStatement {};
    REQUIRE(s.holds<ThrowStatement>());
    s = TryStatement {};
    REQUIRE(s.holds<TryStatement>());
    s = DebuggerStatement {};
    REQUIRE(s.holds<DebuggerStatement>());

}
