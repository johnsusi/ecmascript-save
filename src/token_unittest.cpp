#include "token.h"

#include "catch.hpp"

#include <boost/optional/optional_io.hpp>

TEST_CASE("test")
{
  REQUIRE(Token("null") == Token(nullptr));
  REQUIRE(Token("true") == Token(true));
  REQUIRE(Token("false") == Token(false));
}

TEST_CASE("Keywords")
{
  auto tokens = std::vector<Token>{
      "break",  "case",  "catch",      "continue", "debugger", "default",
      "delete", "do",    "else",       "finally",  "for",      "function",
      "if",     "in",    "instanceof", "new",      "return",   "switch",
      "this",   "throw", "try",        "typeof",   "var",      "void",
      "while",  "with"};

  for (const auto& token : tokens) {

    REQUIRE(token.is_identifier_name());
    REQUIRE(token.is_keyword());

    REQUIRE_FALSE(token.is_empty());
    REQUIRE_FALSE(token.is_identifier());
    REQUIRE_FALSE(token.is_future_reserved_word());
    REQUIRE_FALSE(token.is_punctuator());
    REQUIRE_FALSE(token.is_null_literal());
    REQUIRE_FALSE(token.is_boolean_literal());
    REQUIRE_FALSE(token.is_numeric_literal());
    REQUIRE_FALSE(token.is_string_literal());
    REQUIRE_FALSE(token.is_regular_expression_literal());
  }
}

TEST_CASE("Get/Set")
{
  REQUIRE(Token("get").string_value() == u"get");
}