#include "ast.h"

#include <catch.hpp>

// struct Builder
// {

//   std::vector<Expression> stack;

//   Expression pop()
//   {
//     auto value = stack.back();
//     stack.pop_back();
//     return value;
//   }

//   void push(Expression expression)
//   {
//     stack.push_back(expression);
//   }

//   // void push(Statement statement)
//   // {
//   //   stack.push_back(statement);
//   // }

//   template <typename T, typename... Args>
//   void push(Args&&... args)
//   {
//     push(T { std::forward<Args>(args)... });
//   }

// };

// template <typename R>
// struct expression_visitor : boost::static_visitor<R>
// {
//   template <typename... Ts>
//   R operator()(const boost::variant<Ts...>& value) const
//   {
//     return boost::apply_visitor(*this, value);
//   }
// };

// Expression eval(const Expression& expr)
// {
//   boost::apply_visitor(eval_visitor(), expr);
// }

// // 9.1
// Expression ToPrimitive

// struct eval_visitor : expression_visitor<Expression>
// {
//   Expression operator()(const BinaryExpression& expr) const
//   {
//     if (expr.op == "+")
//     {
//       // 11.6.1.1
//       auto lref = eval(expr.lhs);
//       // 11.6.1.2
//       auto lval = GetValue(lref);
//       // 11.6.1.3
//       auto rref = eval(expr.rhs);
//       // 11.6.1.4
//       auto rval = GetValue(rref);
//       // 11.6.1.5
//       auto lprim = ToPrimitive(lval);
//       // 11.6.1.6
//       auto rprim = ToPrimitive(rval);
//       // 11.6.1.7
//       if (Type(lprim) == string || Type(rprim) == string)
//         return StringLiteral { ToString(lprim).value + ToString(rprim).value };
//       // 11.6.1.8
//       return NumericLiteral { ToNumber(lprim).value + ToNumber(rprim).value };


//       auto lhs = boost::get<NumericLiteral>(expr.lhs);
//       auto rhs = boost::get<NumericLiteral>(expr.rhs);
//       return NumericLiteral { lhs.value + rhs.value };
//     }
//     return NumericLiteral { 0 };
//   }

//   template <typename T>
//   const T& operator()(const T& value) const
//   {
//     return value;
//   }

// };

// TEST_CASE("x")
// {

//   Builder builder;
//   builder.push<NumericLiteral>(1.0);
//   builder.push<NumericLiteral>(23.0);
//   auto rhs = builder.pop();
//   auto lhs = builder.pop();
//   builder.push<BinaryExpression>("+", lhs, rhs);

//   auto expr = builder.pop();

//   auto result = boost::apply_visitor(eval(), expr);
//   auto r = boost::get<NumericLiteral>(result);
//   std::cout << r.value << std::endl;
// }

struct norm
{
  std::string str;
  norm(std::string str) : str(str) {}
  operator const std::string&() const { return str; }
};

std::ostream& operator<<(std::ostream& out, const norm& norm)
{
  return out << normalize(norm.str);
}

TEST_CASE("print")
{
  Program program;
  program.elements.push_back(ReturnStatement{});
  REQUIRE( program == norm(R"(
    Program
      ReturnStatement
  )"));
}