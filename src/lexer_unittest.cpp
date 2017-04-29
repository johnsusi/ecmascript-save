#include "lexer.h"

#include <catch.hpp>

TEST_CASE("7.8.3 Numeric Literals")
{

  REQUIRE_THROWS(Lexer("3in").tokens());

  REQUIRE(Lexer("0x1") == Lexer{Token::numeric_literal(1)});
}

TEST_CASE("Lexical Grammar")
{
  auto expected = Lexer{Token::keyword("var"), Token::identifier("x"),
                        Token::punctuator("="), Token::numeric_literal(1)};

  REQUIRE(Lexer("var x=1") == expected);

  SECTION("HORIZONTAL TAB (U+0009) between any two tokens is allowed")
  {
    REQUIRE(Lexer("\u0009var\u0009x\u0009=\u00091\u0009") == expected);
    REQUIRE(Lexer("\tvar\tx\t=\t1\t") == expected);
    REQUIRE(Lexer("	var\tx	=\t1	") == expected);
  }

  SECTION("VERTICAL TAB (U+000B) between any two tokens is allowed")
  {
    REQUIRE(Lexer("\u000Bvar\u000Bx\u000B=\u000B1\u000B") == expected);
    REQUIRE(Lexer("\vvar\vx\v=\v1\v") == expected);
    REQUIRE(Lexer("var\vx=\v1") == expected);
  }

  SECTION("FORM FEED (U+000C) between any two tokens is allowed")
  {
    REQUIRE(Lexer("\u000Cvar\u000Cx\u000C=\u000C1\u000C") == expected);
    REQUIRE(Lexer("\fvar\fx\f=\f1\f") == expected);
    REQUIRE(Lexer("var\fx=\f1") == expected);
  }

  SECTION("SPACE (U+0020) between any two tokens is allowed")
  {
    REQUIRE(Lexer("\u0020var\u0020x\u0020=\u00201\u0020") == expected);
    REQUIRE(Lexer("\u0020var x\u0020= 1\u0020") == expected);
  }

  SECTION("NO-BREAK SPACE (U+00A0) between any two tokens is allowed")
  {
    REQUIRE(Lexer("\u00A0var\u00A0x\u00A0=\u00A01\u00A0") == expected);
    REQUIRE(Lexer("\u00A0var x\u00A0= 1\u00A0") == expected);
  }
}