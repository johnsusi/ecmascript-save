#include "token.h"

#include "catch.hpp"

TEST_CASE("NullLiteral")
{
  REQUIRE(Token("null") == Token::null_literal());
  REQUIRE(Token::null_literal().is_null_literal());
}

TEST_CASE("BooleanLiteral")
{
  REQUIRE(Token("true") == Token::boolean_literal(true));
  REQUIRE(Token("false") == Token::boolean_literal(false));
  REQUIRE(Token("true") != Token("false"));

  {
    constexpr bool tokens_are_constexpr = Token("true") != Token("false");
    REQUIRE(tokens_are_constexpr);
  }
}

TEST_CASE("IdentifierName")
{

  std::u16string name{u"class"};

  auto token = Token::identifier_name(&name);

  REQUIRE(token.is_reserved_word());
  REQUIRE(token.string_value() == u"class");

  REQUIRE_THROWS(Token("Unknown token"));
}

TEST_CASE("get")
{

  std::u16string name{u"get"};

  auto token = Token::identifier_name(&name);

  REQUIRE(token.is_identifier());
  REQUIRE(token.string_value() == u"get");
  REQUIRE(token == Token("get"));
}

TEST_CASE("UTF16")
{
  REQUIRE(Token("break") == Token(u"break"));
}