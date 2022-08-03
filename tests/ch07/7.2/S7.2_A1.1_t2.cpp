#include <catch2/catch_test_macros.hpp>

#include <ecmascript/runtime/eval.hpp>

struct ECMAScriptValue
{

    bool operator==
};

ECMAScriptValue eval(const std::string_view &source);

TEST_CASE("CHECK#1", "")
{
    REQUIRE(eval("\u0009var\u0009x\u0009=\u00091\u0009") == 1);
}

// //CHECK#2
// eval("\u0009" + "var" + "\u0009" + "x" + "\u0009" + "=" + "\u0009" + "1" + "\u0009");
// if (x !== 1) {
//   $ERROR('#2: eval("\\u0009" + "var" + "\\u0009" + "x" + "\\u0009" + "=" + "\\u0009" + "1" + "\\u0009"); x === 1.
//   Actual: ' + (x));
// }

// //CHECK#3
// eval("\tvar\tx\t=\t1\t");
// if (x !== 1) {
//   $ERROR('#3: eval("\\tvar\\tx\\t=\\t1\\t"); x === 1. Actual: ' + (x));
// }

// //CHECK#4
// eval("\t" + "var" + "\t" + "x" + "\t" + "=" + "\t" + "1" + "\t");
// if (x !== 1) {
//   $ERROR('#4: eval("\\t" + "var" + "\\t" + "x" + "\\t" + "=" + "\\t" + "1" + "\\t"); x === 1. Actual: ' + (x));
// }

// //CHECK#5
// eval("\u0009" + "var" + "\t" + "x" + "\u0009" + "=" + "\t" + "1" + "\u0009");
// if (x !== 1) {
//   $ERROR('#5: eval("\\u0009" + "var" + "\\t" + "x" + "\\u0009" + "=" + "\\t" + "1" + "\\u0009"); x === 1. Actual: ' +
//   (x));
// }
