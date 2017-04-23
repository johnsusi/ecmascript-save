#include "token.h"

#include "catch.hpp"

#include <boost/optional/optional_io.hpp>

TEST_CASE("A compile time token")
{
  Token subject("break");

  SECTION("is a keyword and nothing else")
  {
    REQUIRE(subject.is_keyword() );
    REQUIRE(!subject.is_punctuator() );
    REQUIRE(!subject.is_identifier() );
    REQUIRE(!subject.is_null_literal() );
    REQUIRE(!subject.is_numeric_literal() );
    REQUIRE(!subject.is_string_literal() );
  }

  SECTION("compared to another compile time token with the same content is equal")
  {
    REQUIRE(subject == Token("break"));
  }

  SECTION("compared to an explicit compile time token with the same content is equal")
  {
    REQUIRE(subject == Token::keyword("break"));
  }

  SECTION("compared to a runtime token with the same content is equal")
  {
    std::string s = "break";
    REQUIRE( subject == Token(s.begin(), s.end()) );
  }

  SECTION("compared to a runtime token with the same content in UTF-16 is equal")
  {
    auto source = u"break";
    REQUIRE( subject == Token(source, source + 5) );
  }
}

TEST_CASE("A random string")
{

  // Token subject(u"random string");

  // REQUIRE(subject.is_identifier());

}

TEST_CASE("keywords")
{
  REQUIRE( *Token::keyword("break").to_keyword() == "break" );

}

// TEST_CASE("x")
// {
//   std::vector<std::string> punctuators = {
//     "!", "!=", "!==", "%", "%=", "&", "&&", "&=", "(", ")", "*", "*=", "+",
//     "++", "+=", ",", "-", "--", "-=", ".", ":", ";", "<", "<<", "<<=", "<=",
//     "=", "==", "===", ">", ">=", ">>", ">>=", ">>>", ">>>=", "?", "[", "]", "^",
//     "^=", "{", "|", "|=", "||", "}", "~"
//   };

//   while (!punctuators.empty()) {
//     Token subject { Token::Type::PUNCTUATOR, punctuators.back() };
//     punctuators.pop_back();

//     REQUIRE(subject.is_punctuator());

//     for (auto i : punctuators) REQUIRE(subject != i);

//   }

// }
