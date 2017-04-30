#include "ast.h"

#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/core/demangle.hpp>

std::string normalize(const std::string &s)
{
  using namespace std;
  using namespace boost;
  vector<string> rows;
  split(rows, s, is_any_of("\n"), token_compress_on);

  auto it = rows.begin();
  int leading_ws = 0;
  for (; it != rows.end(); ++it) {
    auto n = it->find_first_not_of(' ');
    if (n != it->npos) {
      leading_ws = n;
      break;
    }
  }

  stringstream result;

  for (; it != rows.end(); ++it) {
    auto n = it->find_first_not_of(' ');
    if (n == it->npos) continue;
    result << it->substr(leading_ws) << "\n";
  }
  return result.str();
}

// template <typename T> void typeof(const T &);

// using namespace ast;
// struct PrintVisitor : boost::static_visitor<std::string> {

//   std::string indent;
//   PrintVisitor(std::string indent = "") : indent(indent) {}

//   template <typename... Ts>
//   std::string apply(const boost::variant<Ts...> &value,
//                     bool inc_indent = true) const
//   {
//     return boost::apply_visitor(PrintVisitor(indent + (inc_indent ? "  " :
//     "")),
//                                 value);
//   }

//   template <typename T>
//   std::string apply(const T &value, bool inc_indent = true) const
//   {

//     return PrintVisitor(indent + (inc_indent ? "  " : ""))(value);
//   }

//   template <typename T>
//   std::string apply(const boost::optional<T> &value,
//                     bool inc_indent = true) const
//   {
//     return value ? apply(*value, inc_indent) : std::string{};
//   }

//   template <typename... Ts>
//   std::string operator()(const boost::variant<Ts...> &value) const
//   {
//     return boost::apply_visitor(PrintVisitor(indent + ""), value);
//   }

//   template <typename T> std::string operator()(const std::vector<T> &list)
//   const
//   {
//     std::stringstream result;
//     for (const auto &value : list)
//       result << apply(value, false);
//     return result.str();
//   }

//   auto operator()(const NullLiteral &literal) const
//   {
//     return apply("null", false);
//   }

//   auto operator()(const BooleanLiteral &literal) const
//   {
//     return apply(literal.value ? "true" : "false", false);
//   }

//   auto operator()(const NumericLiteral &literal) const
//   {
//     std::stringstream buffer;
//     buffer << indent << literal.value << "\n";
//     return buffer.str();
//   }

//   auto operator()(const StringLiteral &literal) const
//   {
//     return apply(literal.value, false);
//   }

//   auto operator()(const Identifier &identifier) const
//   {
//     return apply(identifier.value, false);
//   }

//   auto operator()(const Arguments &args) const
//   {
//     return apply(args.list, false);
//   }

//   auto operator()(const MemberExpression &expr) const
//   {
//     return indent + ".\n" + apply(expr.object) + apply(expr.property);
//   }

//   auto operator()(const NewExpression &expr) const
//   {
//     return indent + "new\n" + apply(expr.callee) + apply(expr.arguments);
//   }

//   auto operator()(const CallExpression &expr) const
//   {
//     return indent + "()\n" + apply(expr.callee) + apply(expr.arguments);
//   }

//   auto operator()(const PostfixExpression &expr) const
//   {
//     return indent + expr.op + "(int)\n" + apply(expr.lhs);
//   }

//   auto operator()(const UnaryExpression &expr) const
//   {
//     return indent + expr.op + "\n" + apply(expr.rhs);
//   }

//   auto operator()(const BinaryExpression &expr) const
//   {
//     return indent + expr.op + "\n" + apply(expr.lhs) + apply(expr.rhs);
//   }

//   auto operator()(const Block &stmt) const
//   {
//     return indent + "Block\n" + apply(stmt.body);
//   }

//   auto operator()(const EmptyStatement &) const
//   {
//     return indent + "EmptyStatement\n";
//   }

//   auto operator()(const ExpressionStatement &stmt) const
//   {
//     return indent + "ExpressionStatement\n" + apply(stmt.expression);
//   }

//   auto operator()(const IfStatement &stmt) const
//   {
//     return indent + "IfStatement\n" + apply(stmt.test) +
//            apply(stmt.consequent) + apply(stmt.alternate);
//   }

//   auto operator()(const DoWhileStatement &stmt) const
//   {
//     return indent + "DoWhileStatement\n" + apply(stmt.body) +
//     apply(stmt.test);
//   }

//   auto operator()(const WhileStatement &stmt) const
//   {
//     return indent + "WhileStatement\n" + apply(stmt.test) + apply(stmt.body);
//   }

//   auto operator()(const ForStatement &) const
//   {
//     return indent + "ForStatement\n";
//   }

//   auto operator()(const ForInStatement &) const
//   {
//     return indent + "ForInStatement\n";
//   }

//   auto operator()(const ContinueStatement &stmt) const
//   {
//     return indent + "ContinueStatement\n" + apply(stmt.label);
//   }

//   auto operator()(const BreakStatement &stmt) const
//   {
//     return indent + "BreakStatement\n" + apply(stmt.label);
//   }

//   auto operator()(const ReturnStatement &stmt) const
//   {
//     return indent + "ReturnStatement\n" + apply(stmt.argument);
//   }

//   auto operator()(const WithStatement &stmt) const
//   {
//     return indent + "WithStatement\n" + apply(stmt.object) +
//     apply(stmt.body);
//   }

//   auto operator()(const LabelledStatement &stmt) const
//   {
//     return indent + "LabelledStatement\n" + apply(stmt.label) +
//            apply(stmt.body);
//   }

//   auto operator()(const SwitchStatement &stmt) const
//   {
//     return indent + "SwitchStatement\n" + apply(stmt.discriminant);
//   }

//   auto operator()(const ThrowStatement &stmt) const
//   {
//     return indent + "ThrowStatement\n";
//   }

//   auto operator()(const TryStatement &stmt) const
//   {
//     return indent + "TryStatement\n";
//   }

//   auto operator()(const DebuggerStatement &stmt) const
//   {
//     return indent + "DebuggerStatement\n";
//   }

//   auto operator()(const FunctionDeclaration &decl) const
//   {
//     return indent + "FunctionDeclaration\n";
//   }

//   auto operator()(const Program &program) const
//   {
//     return indent + "Program\n" + apply(program.elements);
//   }

//   auto operator()(const std::string &str) const { return indent + str + "\n";
//   }

//   auto operator()(const std::u16string &str) const
//   {
//     return indent + std::string{str.begin(), str.end()} + "\n";
//   }

//   auto operator()(double value) const
//   {
//     std::stringstream buffer;
//     buffer << indent << value << "\n";
//     return buffer.str();
//   }

//   auto operator()(const char *value) const { return indent + value + "\n"; }

//   template <typename T> auto operator()(const T &what) const
//   {
//     auto type_name = boost::core::demangle(typeid(what).name());
//     return indent + "Unknown: " + type_name + "\n";
//   }
// };

// std::string to_string(const Program &program)
// {
//   return PrintVisitor()(program);
// }

// bool operator==(const Program &program, const std::string &str)
// {
//   return to_string(program) == normalize(str);
// }

// std::ostream &operator<<(std::ostream &out, const Program &program)
// {
//   return out << to_string(program);
// }

using namespace ast;

template <typename Callable> struct CallableVisitor : Visitor {

  Callable callback;
  CallableVisitor(Callable callback) : callback(callback) {}

  template <typename T> void apply(const T *node)
  {
    if (node) node->accept(*this);
  }

  template <typename T, typename... Args>
  void call_and_apply(const T &node, Args... args)
  {
    callback(node);
    (void)(int[]){(apply(std::forward<Args>(args)), 0)...};
      // https://twitter.com/SeanParent/status/558330478541803522
  }

  void operator()(const This &node) override { callback(node); }
  void operator()(const Identifier &node) override { callback(node); }
  void operator()(const NullLiteral &node) override { callback(node); }
  void operator()(const BooleanLiteral &node) override { callback(node); }
  void operator()(const NumericLiteral &node) override { callback(node); }
  void operator()(const StringLiteral &node) override { callback(node); }
  void operator()(const RegularExpressionLiteral &node) override
  {
    callback(node);
  }
  void operator()(const ArrayLiteral &node) override { callback(node); }
  void operator()(const ObjectLiteral &node) override { callback(node); }
  void operator()(const MemberExpression &node) override { callback(node); }
  void operator()(const NewExpression &node) override { callback(node); }
  void operator()(const CallExpression &node) override { callback(node); }
  void operator()(const PostfixExpression &node) override { callback(node); }
  void operator()(const UnaryExpression &node) override { callback(node); }
  void operator()(const BinaryExpression &node) override
  {
    call_and_apply(node, node.lhs, node.rhs);
  }
  void operator()(const ConditionalExpression &node) override
  {
    call_and_apply(node, node.test, node.consequent, node.alternate);
  }
  void operator()(const FunctionExpression &node) override { callback(node); }

  void operator()(const Block &node) override { call_and_apply(node, node.body); }
  void operator()(const VariableStatement &node) override { callback(node); }
  void operator()(const EmptyStatement &node) override { callback(node); }
  void operator()(const ExpressionStatement &node) override
  {
    call_and_apply(node, node.expression);
  }
  void operator()(const IfStatement &node) override
  {
    call_and_apply(node, node.test, node.consequent, node.alternate);
  }
  void operator()(const DoWhileStatement &node) override { callback(node); }
  void operator()(const WhileStatement &node) override { callback(node); }
  void operator()(const ForStatement &node) override { callback(node); }
  void operator()(const ForInStatement &node) override { callback(node); }
  void operator()(const ContinueStatement &node) override { call_and_apply(node, node.label); }
  void operator()(const BreakStatement &node) override { callback(node); }
  void operator()(const ReturnStatement &node) override { callback(node); }
  void operator()(const WithStatement &node) override { callback(node); }
  void operator()(const LabelledStatement &node) override { callback(node); }
  void operator()(const SwitchStatement &node) override { callback(node); }
  void operator()(const ThrowStatement &node) override { callback(node); }
  void operator()(const TryStatement &node) override { callback(node); }
  void operator()(const DebuggerStatement &node) override { callback(node); }

  void operator()(const CaseClause &node) override { callback(node); }
  void operator()(const DefaultClause &node) override { callback(node); }

  void operator()(const FunctionDeclaration &node) override { call_and_apply(node, node.body); }
  // void operator()(const FunctionBody &node) override { callback(node); }

  void operator()(const VariableDeclaration &node) override { callback(node); }
  void operator()(const Elision &node) override { callback(node); }
  void operator()(const PropertyName &node) override { callback(node); }
  void operator()(const PropertyAssignment &node) override { callback(node); }
  void operator()(const Arguments &node) override { callback(node); }
};

template <typename F> CallableVisitor<F> make_visitor(F callback)
{
  return CallableVisitor<F>{callback};
}

std::string to_string(const ast::Program &program)
{
  std::stringstream buf;
  accept(program, make_visitor([&](auto node) {
           buf << boost::core::demangle(typeid(decltype(node)).name()) << "\n";
         }));
  return buf.str();
}

namespace ast {
void accept(const Program &program, Visitor &&visitor)
{
  if (program.body) program.body->accept(visitor);
  // for (auto child : *program.body) accept(child, visitor);
}

} // namespace ast