#include "to_string.h"

#include <catch.hpp>

TEST_CASE("ToString") {

  REQUIRE(ToString(0.0) == "0");
  REQUIRE(ToString(+0.0) == "0");
  REQUIRE(ToString(-0.0) == "-0");

  REQUIRE(ToString(1e0) == "1");
  REQUIRE(ToString(1e1) == "10");
  REQUIRE(ToString(1e2) == "100");
  REQUIRE(ToString(1e3) == "1000");
  REQUIRE(ToString(1e4) == "10000");
  REQUIRE(ToString(1e5) == "100000");
  REQUIRE(ToString(1e6) == "1000000");
  REQUIRE(ToString(1e7) == "10000000");
  REQUIRE(ToString(1e8) == "100000000");
  REQUIRE(ToString(1e9) == "1000000000");
  REQUIRE(ToString(1e10) == "10000000000");
  REQUIRE(ToString(1e11) == "100000000000");
  REQUIRE(ToString(1e12) == "1000000000000");
  REQUIRE(ToString(1e13) == "10000000000000");
  REQUIRE(ToString(1e14) == "100000000000000");
  REQUIRE(ToString(1e15) == "1000000000000000");
  REQUIRE(ToString(1e16) == "10000000000000000");
  REQUIRE(ToString(1e17) == "100000000000000000");
  REQUIRE(ToString(1e18) == "1000000000000000000");
  REQUIRE(ToString(1e19) == "10000000000000000000");
  REQUIRE(ToString(1e20) == "100000000000000000000");
  REQUIRE(ToString(1e21) == "1e+21");
  REQUIRE(ToString(1e22) == "1e+22");
  REQUIRE(ToString(1e23) == "1e+23");
  REQUIRE(ToString(1e24) == "1e+24");
  REQUIRE(ToString(1e25) == "1e+25");
  REQUIRE(ToString(1e26) == "1e+26");
  REQUIRE(ToString(1e27) == "1e+27");
  REQUIRE(ToString(1e28) == "1e+28");
  REQUIRE(ToString(1e29) == "1e+29");

  REQUIRE(ToString(-1e21) == "-1e+21");

  REQUIRE(ToString(1e-1) == "0.1");
  REQUIRE(ToString(1e-2) == "0.01");
  REQUIRE(ToString(1e-3) == "0.001");
  REQUIRE(ToString(1e-4) == "0.0001");
  REQUIRE(ToString(1e-5) == "0.00001");
  REQUIRE(ToString(1e-6) == "0.000001");
  REQUIRE(ToString(1e-7) == "1e-7");
  REQUIRE(ToString(1e-8) == "1e-8");
  REQUIRE(ToString(1e-9) == "1e-9");

  REQUIRE(ToString(-9007199254740991) == "-9007199254740991");
  REQUIRE(ToString(9007199254740991) == "9007199254740991");

  REQUIRE(ToString(5e-324) == "5e-324");
  REQUIRE(ToString(1.79E+308) == "1.79e+308");

  REQUIRE(ToString(4.9406564584124654417656879286822e-324) == "5e-324");
  REQUIRE(ToString(8.98846567431158e+307) == "8.98846567431158e+307");
  REQUIRE(ToString(1.7976931348623157e+308) == "1.7976931348623157e+308");
  REQUIRE(ToString(-8.98846567431158e+307) == "-8.98846567431158e+307");

  REQUIRE(ToString(1.0 / 0.0) == "Infinity");
  REQUIRE(ToString(1.0 / -0.0) == "-Infinity");
  REQUIRE(ToString(-1.0 / -0.0) == "Infinity");
  REQUIRE(ToString(-1.0 / 0.0) == "-Infinity");
}