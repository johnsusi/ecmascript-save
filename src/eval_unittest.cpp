#include "token.h"
#include "type.h"

#include "catch.hpp"

#include <iostream>

struct Context {
};

struct Expression {

  virtual Type eval(Context&) const = 0;
};

struct NumericLiteral : Expression {
  Number value;
  NumericLiteral(Number value) : value(value)
  {
  }
  Type eval(Context& context) const final
  {
    return value;
  }
};

struct BinaryExpression : Expression {
  Token       op;
  Expression* left;
  Expression* right;

  BinaryExpression(Token op, Expression* left, Expression* right)
      : op(op), left(left), right(right)
  {
  }

  Type eval(Context& context) const final
  {
    auto lref = left->eval(context);
    auto rref = right->eval(context);

    return {};
    // auto lprim = ToPrimitive(lref);
    // auto rprim = ToPrimitive(rref);

    // if (op == "+") {
    //   return ToNumber(lprim) + ToNumber(rprim);
    // }
    // else if (op == "-") {
    //   return ToNumber(lprim) - ToNumber(rprim);
    // }
    // else if (op == "*") {
    //   return ToNumber(lprim) * ToNumber(rprim);
    // }
    // else if (op == "/") {
    //   return ToNumber(lprim) / ToNumber(rprim);
    // }
    // return {};
  }
};

TEST_CASE("eval")
{

  NumericLiteral   a{1.0};
  NumericLiteral   b{2.0};
  NumericLiteral   c{4.0};
  BinaryExpression d("+", &a, &b);
  BinaryExpression e("-", &d, &c);

  auto program = &e;

  Context context;

  // auto result = program->eval(context);

  // auto str = static_cast<std::string>(ToString(result));

  // REQUIRE(str == "-1");

  // std::cout << "result: " << str << "\n";
}