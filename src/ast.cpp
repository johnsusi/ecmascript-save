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
    if (n == it->npos)
      continue;
    result << it->substr(leading_ws) << "\n";
  }
  return result.str();
}

template <typename T> void typeof(const T &);

struct PrintVisitor : boost::static_visitor<std::string> {

  std::string indent;
  PrintVisitor(std::string indent = "") : indent(indent) {}

  template <typename... Ts>
  std::string apply(const boost::variant<Ts...> &value,
                    bool inc_indent = true) const
  {
    return boost::apply_visitor(PrintVisitor(indent + (inc_indent ? "  " : "")),
                                value);
  }

  template <typename T>
  std::string apply(const T &value, bool inc_indent = true) const
  {

    return PrintVisitor(indent + (inc_indent ? "  " : ""))(value);
  }

  template <typename T>
  std::string apply(const boost::optional<T> &value,
                    bool inc_indent = true) const
  {
    return value ? apply(*value, inc_indent) : std::string{};
  }

  template <typename... Ts>
  std::string operator()(const boost::variant<Ts...> &value) const
  {
    return boost::apply_visitor(PrintVisitor(indent + ""), value);
  }

  template <typename T> std::string operator()(const std::vector<T> &list) const
  {
    std::stringstream result;
    for (const auto &value : list)
      result << apply(value, false);
    return result.str();
  }

  auto operator()(const NullLiteral &literal) const
  {
    return apply("null", false);
  }

  auto operator()(const BooleanLiteral &literal) const
  {
    return apply(literal.value ? "true" : "false", false);
  }

  auto operator()(const NumericLiteral &literal) const
  {
    std::stringstream buffer;
    buffer << indent << literal.value << "\n";
    return buffer.str();
  }

  auto operator()(const StringLiteral &literal) const
  {
    return apply(literal.value, false);
  }

  auto operator()(const Identifier &identifier) const
  {
    return apply(identifier.value, false);
  }

  auto operator()(const Arguments &args) const
  {
    return apply(args.list, false);
  }

  auto operator()(const MemberExpression &expr) const
  {
    return indent + ".\n" + apply(expr.object) + apply(expr.property);
  }

  auto operator()(const NewExpression &expr) const
  {
    return indent + "new\n" + apply(expr.callee) + apply(expr.arguments);
  }

  auto operator()(const CallExpression &expr) const
  {
    return indent + "()\n" + apply(expr.callee) + apply(expr.arguments);
  }

  auto operator()(const PostfixExpression &expr) const
  {
    return indent + expr.op + "(int)\n" + apply(expr.lhs);
  }

  auto operator()(const UnaryExpression &expr) const
  {
    return indent + expr.op + "\n" + apply(expr.rhs);
  }

  auto operator()(const BinaryExpression &expr) const
  {
    return indent + expr.op + "\n" + apply(expr.lhs) + apply(expr.rhs);
  }

  auto operator()(const Block &stmt) const
  {
    return indent + "Block\n" + apply(stmt.body);
  }

  auto operator()(const EmptyStatement &) const
  {
    return indent + "EmptyStatement\n";
  }

  auto operator()(const ExpressionStatement &stmt) const
  {
    return indent + "ExpressionStatement\n" + apply(stmt.expression);
  }

  auto operator()(const IfStatement &stmt) const
  {
    return indent + "IfStatement\n" + apply(stmt.test) +
           apply(stmt.consequent) + apply(stmt.alternate);
  }

  auto operator()(const DoWhileStatement &stmt) const
  {
    return indent + "DoWhileStatement\n" + apply(stmt.body) + apply(stmt.test);
  }

  auto operator()(const WhileStatement &stmt) const
  {
    return indent + "WhileStatement\n" + apply(stmt.test) + apply(stmt.body);
  }

  auto operator()(const ForStatement &) const
  {
    return indent + "ForStatement\n";
  }

  auto operator()(const ForInStatement &) const
  {
    return indent + "ForInStatement\n";
  }

  auto operator()(const ContinueStatement &stmt) const
  {
    return indent + "ContinueStatement\n" + apply(stmt.label);
  }

  auto operator()(const BreakStatement &stmt) const
  {
    return indent + "BreakStatement\n" + apply(stmt.label);
  }

  auto operator()(const ReturnStatement &stmt) const
  {
    return indent + "ReturnStatement\n" + apply(stmt.argument);
  }

  auto operator()(const WithStatement &stmt) const
  {
    return indent + "WithStatement\n" + apply(stmt.object) + apply(stmt.body);
  }

  auto operator()(const LabelledStatement &stmt) const
  {
    return indent + "LabelledStatement\n" + apply(stmt.label) +
           apply(stmt.body);
  }

  auto operator()(const SwitchStatement &stmt) const
  {
    return indent + "SwitchStatement\n" + apply(stmt.discriminant);
  }

  auto operator()(const ThrowStatement &stmt) const
  {
    return indent + "ThrowStatement\n";
  }

  auto operator()(const TryStatement &stmt) const
  {
    return indent + "TryStatement\n";
  }

  auto operator()(const DebuggerStatement &stmt) const
  {
    return indent + "DebuggerStatement\n";
  }

  auto operator()(const FunctionDeclaration &decl) const
  {
    return indent + "FunctionDeclaration\n";
  }

  auto operator()(const Program &program) const
  {
    return indent + "Program\n" + apply(program.elements);
  }

  auto operator()(const std::string &str) const { return indent + str + "\n"; }

  auto operator()(const std::u16string &str) const
  {
    return indent + std::string{str.begin(), str.end()} + "\n";
  }

  auto operator()(double value) const
  {
    std::stringstream buffer;
    buffer << indent << value << "\n";
    return buffer.str();
  }

  auto operator()(const char *value) const { return indent + value + "\n"; }

  template <typename T> auto operator()(const T &what) const
  {
    auto type_name = boost::core::demangle(typeid(what).name());
    return indent + "Unknown: " + type_name + "\n";
  }
};

std::string to_string(const Program &program)
{
  return PrintVisitor()(program);
}

bool operator==(const Program &program, const std::string &str)
{
  return to_string(program) == normalize(str);
}

std::ostream &operator<<(std::ostream &out, const Program &program)
{
  return out << to_string(program);
}
