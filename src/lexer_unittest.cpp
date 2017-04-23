#include "lexer.h"

#include <catch.hpp>

TEST_CASE("A simple lexer")
{

  auto lexer = make_lexer("1+1");

  REQUIRE(lexer.input_element_div().is_token());
  REQUIRE(lexer.input_element_div().is_token());
  REQUIRE(lexer.input_element_div().is_token());
  REQUIRE(lexer.input_element_div().is_empty());
}

TEST_CASE("A simple lexer with whitespace")
{

  auto lexer = make_lexer(" 1 + 1 ");

  REQUIRE(lexer.input_element_div().is_white_space());
  REQUIRE(lexer.input_element_div().is_token());
  REQUIRE(lexer.input_element_div().is_white_space());
  REQUIRE(lexer.input_element_div().is_token());
  REQUIRE(lexer.input_element_div().is_white_space());
  REQUIRE(lexer.input_element_div().is_token());
  REQUIRE(lexer.input_element_div().is_white_space());
  REQUIRE(lexer.input_element_div().is_empty());
}

TEST_CASE("A simple lexer with whitespace and line terminators")
{

  auto lexer = make_lexer(" 1\n+\n1 ");

  REQUIRE(lexer.input_element_div().is_white_space());
  REQUIRE(lexer.input_element_div().is_token());
  REQUIRE(lexer.input_element_div().is_line_terminator());
  REQUIRE(lexer.input_element_div().is_token());
  REQUIRE(lexer.input_element_div().is_line_terminator());
  REQUIRE(lexer.input_element_div().is_token());
  REQUIRE(lexer.input_element_div().is_white_space());
  REQUIRE(lexer.input_element_div().is_empty());
}

TEST_CASE("A simple lexer with single line comments")
{

  auto lexer = make_lexer("1//comment\n+1");

  REQUIRE(lexer.input_element_div().is_token());
  REQUIRE(lexer.input_element_div().is_comment());
  REQUIRE(lexer.input_element_div().is_line_terminator());
  REQUIRE(lexer.input_element_div().is_token());
  REQUIRE(lexer.input_element_div().is_token());
  REQUIRE(lexer.input_element_div().is_empty());
}

TEST_CASE("A simple lexer with multi line comments")
{

  auto lexer = make_lexer("1/*comment\n*/+1");

  SECTION("should not have a line terminator input element")
  {
    REQUIRE(lexer.input_element_div().is_token());
    REQUIRE(lexer.input_element_div().is_comment());
    REQUIRE(lexer.input_element_div().is_token());
    REQUIRE(lexer.input_element_div().is_token());
    REQUIRE(lexer.input_element_div().is_empty());
  }

  SECTION("should behave like a line terminator")
  {
    REQUIRE(lexer.input_element_div().is_token());
    REQUIRE(lexer.input_element_div().has_line_terminator());
    REQUIRE(lexer.input_element_div().is_token());
    REQUIRE(lexer.input_element_div().is_token());
    REQUIRE(lexer.input_element_div().is_empty());
  }
}