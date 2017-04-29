#include "input_element.h"

#include "catch.hpp"

TEST_CASE("some test") { auto input_element = InputElement::empty(); }

TEST_CASE("A line terminator has a line terminator")
{
  REQUIRE(InputElement::line_terminator().has_line_terminator());
}

TEST_CASE("A comment with one or more line terminators has a line terminator")
{
  REQUIRE(InputElement::comment(u"hello\nworld").has_line_terminator());
}

TEST_CASE("A comment without line terminators has not a line terminator")
{
  REQUIRE(!InputElement::comment(u"hello, world").has_line_terminator());
}
