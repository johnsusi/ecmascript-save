#include "lexer.h"
#include "parser.h"

#include <catch.hpp>

#include <boost/optional/optional_io.hpp>

Program parse(std::string source)
{
  auto lexer = make_lexer(source);
  auto tokens = lexer.tokens();
  auto parser = make_parser(tokens);
  return parser.parse();
}

bool operator==(const boost::optional<Program> &lhs, const std::string &rhs)
{
  return lhs && *lhs == rhs;
}

TEST_CASE("7.9.2 Automatic Semicolon Insertion")
{

  REQUIRE_THROWS(parse("{ 1 2 } 3"));

  REQUIRE(parse("{ 1\n2 } 3") == R"(
    Program
      Block
        ExpressionStatement
          1
        ExpressionStatement
          2
      ExpressionStatement
        3
  )");

  REQUIRE_THROWS(parse("for (a; b\n)"));

  REQUIRE(parse("return\na+b") == R"(
    Program
      ReturnStatement
      ExpressionStatement
        +
          a
          b
  )");

  REQUIRE(parse("a=b\n++c") == R"(
    Program
      ExpressionStatement
        =
          a
          b
      ExpressionStatement
        ++
          c
  )");
}