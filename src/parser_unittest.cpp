#include "ast_visitor.h"
#include "json_visitor.h"
#include "lexer.h"
#include "parser.h"

#include <iostream>

#include <catch.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/optional/optional_io.hpp>

Program parse(std::string source)
{
  auto lexer  = make_lexer(source);
  auto tokens = lexer.tokens();
  auto parser = make_parser(tokens);
  return parser.parse();
}

bool operator==(const Program &program, const std::string &expected)
{
  SimplifiedYAMLVisitor yaml;
  program.accept(yaml);
  auto lhs = yaml.str();
  auto rhs = left_align_text(expected);
  boost::trim_right(lhs);
  boost::trim_right(rhs);
  return lhs == rhs;
}

std::ostream &operator<<(std::ostream &out, const Program &program)
{
  SimplifiedYAMLVisitor yaml;
  program.accept(yaml);
  return out << yaml.str();
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