#include "lexer.h"

#include <catch.hpp>

TEST_CASE("7.8.3 Numeric Literals")
{

  REQUIRE_THROWS(Lexer("3in").tokens());

  REQUIRE(Lexer("0x1") == Lexer{Token::numeric_literal(1)});

  REQUIRE(Lexer("0e1") == Lexer{Token::numeric_literal(0)});
  REQUIRE(Lexer("1e1") == Lexer{Token::numeric_literal(10)});
  REQUIRE(Lexer("2e1") == Lexer{Token::numeric_literal(20)});
  REQUIRE(Lexer("3e1") == Lexer{Token::numeric_literal(30)});
  REQUIRE(Lexer("4e1") == Lexer{Token::numeric_literal(40)});
  REQUIRE(Lexer("5e1") == Lexer{Token::numeric_literal(50)});
  REQUIRE(Lexer("6e1") == Lexer{Token::numeric_literal(60)});
  REQUIRE(Lexer("7e1") == Lexer{Token::numeric_literal(70)});
  REQUIRE(Lexer("8e1") == Lexer{Token::numeric_literal(80)});
  REQUIRE(Lexer("9e1") == Lexer{Token::numeric_literal(90)});

  REQUIRE(Lexer("0e-1") == Lexer{Token::numeric_literal(0)});
  REQUIRE(Lexer("1e-1") == Lexer{Token::numeric_literal(0.1)});
  REQUIRE(Lexer("2e-1") == Lexer{Token::numeric_literal(0.2)});
  REQUIRE(Lexer("3e-1") == Lexer{Token::numeric_literal(0.3)});
  REQUIRE(Lexer("4e-1") == Lexer{Token::numeric_literal(0.4)});
  REQUIRE(Lexer("5e-1") == Lexer{Token::numeric_literal(0.5)});
  REQUIRE(Lexer("6e-1") == Lexer{Token::numeric_literal(0.6)});
  REQUIRE(Lexer("7e-1") == Lexer{Token::numeric_literal(0.7)});
  REQUIRE(Lexer("8e-1") == Lexer{Token::numeric_literal(0.8)});
  REQUIRE(Lexer("9e-1") == Lexer{Token::numeric_literal(0.9)});

  REQUIRE(Lexer("0e+1") == Lexer{Token::numeric_literal(0)});
  REQUIRE(Lexer("1e+1") == Lexer{Token::numeric_literal(10)});
  REQUIRE(Lexer("2e+1") == Lexer{Token::numeric_literal(20)});
  REQUIRE(Lexer("3e+1") == Lexer{Token::numeric_literal(30)});
  REQUIRE(Lexer("4e+1") == Lexer{Token::numeric_literal(40)});
  REQUIRE(Lexer("5e+1") == Lexer{Token::numeric_literal(50)});
  REQUIRE(Lexer("6e+1") == Lexer{Token::numeric_literal(60)});
  REQUIRE(Lexer("7e+1") == Lexer{Token::numeric_literal(70)});
  REQUIRE(Lexer("8e+1") == Lexer{Token::numeric_literal(80)});
  REQUIRE(Lexer("9e+1") == Lexer{Token::numeric_literal(90)});
}

template <typename... Args>
std::vector<Token> make_string_literals(Args&&... args)
{
  return {Token::string_literal(std::forward<Args>(args))...};
}

TEST_CASE("String Literals")
{

  auto expected = Lexer{make_string_literals(
      u"\b", u"\b", u"\b", u"\b", u"\b", u"\b", u"\t", u"\t", u"\t", u"\t",
      u"\t", u"\t", u"\n", u"\n", u"\n", u"\n", u"\n", u"\n", u"\v", u"\v",
      u"\v", u"\v", u"\v", u"\v", u"\f", u"\f", u"\f", u"\f", u"\f", u"\f",
      u"\r", u"\r", u"\r", u"\r", u"\r", u"\r", u"\"", u"\"", u"\"", u"\"",
      u"\"", u"\"", u"\'", u"\'", u"\'", u"\'", u"\'", u"\'", u"\\", u"\\",
      u"\\", u"\\", u"\\", u"\\")};

  REQUIRE(Lexer(R"(
    "\b" "\u0008" "\x08" '\b' '\u0008' '\x08'
    "\t" "\u0009" "\x09" '\t' '\u0009' '\x09'
    "\n" "\u000A" "\x0A" '\n' '\u000A' '\x0A'
    "\v" "\u000B" "\x0B" '\v' '\u000B' '\x0B'
    "\f" "\u000C" "\x0C" '\f' '\u000C' '\x0C'
    "\r" "\u000D" "\x0D" '\r' '\u000D' '\x0D'
    "\"" "\u0022" "\x22" '\"' '\u0022' '\x22'
    "\'" "\u0027" "\x27" '\'' '\u0027' '\x27'
    "\\" "\u005C" "\x5C" '\\' '\u005C' '\x5C'
  )") == expected);
}

template <typename... Args>
std::vector<Token> make_punctuators(Args&&... args)
{
  return {Token::punctuator(std::forward<Args>(args))...};
}

TEST_CASE("Punctuators")
{

  auto expected = Lexer(make_punctuators(
      "!", "!=", "!==", "%", "%=", "&", "&&", "&=", "(", ")", "*", "*=", "+",
      "++", "+=", ",", "-", "--", "-=", ".", ":", ";", "<", "<<",
      "<<=", "<=", "=", "==", "===", ">", ">=", ">>", ">>=", ">>>", ">>>=", "?",
      "[", "]", "^", "^=", "{", "|", "|=", "||", "}", "~"));

  REQUIRE(Lexer(R"(
    ! != !== % %= & && &= ( ) * *= + ++ += , - -- -=
    . : ; < << <<= <= = == === > >= >> >>= >>> >>>=
    ? [ ] ^ ^= { | |= || } ~
  )") == expected);
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
    REQUIRE(Lexer("\tvar\tx	=\t1	") == expected);
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
