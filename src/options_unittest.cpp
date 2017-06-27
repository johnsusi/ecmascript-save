#include "options.h"

#include "catch.hpp"

using namespace std::string_literals;

TEST_CASE("Options")
{
  Options options;

  options["foo"] = true;
  options["Foo"] = false;
  options["bar"] = 1;
  options["Bar"] = 3.14159265;
  options["baz"] = "Hello, World";
  options["Baz"] = std::string{"Hello, World"};

  REQUIRE(options["foo"] == true);
  REQUIRE(options["Foo"] == false);
  REQUIRE(options["bar"] == 1);
  REQUIRE(options["Bar"] == 3.14159265);
  REQUIRE(options["baz"] == std::string{"Hello, World"});
  REQUIRE(options["Baz"] == std::string{"Hello, World"});
}

TEST_CASE("Overwrite Options")
{
  Options options;

  options["foo"] = true;
  REQUIRE(options["foo"] == true);
  options["foo"] = false;
  REQUIRE(options["foo"] == false);
  options["foo"] = 1;
  REQUIRE(options["foo"] == 1);
  options["foo"] = 3.14159265;
  REQUIRE(options["foo"] == 3.14159265);
  options["foo"] = "Hello, World";
  REQUIRE(options["foo"] == std::string{"Hello, World"});
  options["foo"] = std::string{"Hello, World"};
  REQUIRE(options["foo"] == std::string{"Hello, World"});
}

TEST_CASE("Copying Options")
{
  Options options;

  options["foo"] = true;
  options["Foo"] = false;
  options["bar"] = 1;
  options["Bar"] = 3.14159265;
  options["baz"] = "Hello, World";
  options["Baz"] = std::string{"Hello, World"};

  auto test = [](const Options& options) {
    REQUIRE(options["foo"] == true);
    REQUIRE(options["Foo"] == false);
    REQUIRE(options["bar"] == 1);
    REQUIRE(options["Bar"] == 3.14159265);
    REQUIRE(options["baz"] == std::string{"Hello, World"});
    REQUIRE(options["Baz"] == std::string{"Hello, World"});
  };

  Options a(options);
  test(a);

  Options b = options;
  test(b);

  Options c(std::move(options));
  test(c);
}

TEST_CASE("Initializer list")
{
  Options options{
      {"foo", true},       {"Foo", false},           {"bar", 1},
      {"Bar", 3.14159265}, {"baz", "Hello, World"s}, {"Baz", "Hello, World"s}};

  REQUIRE(options["foo"] == true);
  REQUIRE(options["Foo"] == false);
  REQUIRE(options["bar"] == 1);
  REQUIRE(options["Bar"] == 3.14159265);
  REQUIRE(options["baz"] == "Hello, World"s);
  REQUIRE(options["Baz"] == "Hello, World"s);
}
