#include "ast.h"
#include "util.h"

#include <sstream>
#include <string>
#include <vector>

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

// template <typename Callable>
// struct CallableVisitor : Visitor {

//   using Visitor::operator();

//   Callable callback;
//   CallableVisitor(Callable callback) : callback(callback) {}

//   template <typename T>
//   void call(const T &node)
//   {
//     callback(node, [&](auto child) {
//       if (child) child->accept(*this);
//     });
//     // (void)(int[]){(apply(std::forward<Args>(args)), 0)...};
//     // https://twitter.com/SeanParent/status/558330478541803522
//   }

//   void operator()(const This &node) override { call(node); }
//   void operator()(const Identifier &node) override { call(node); }
//   void operator()(const NullLiteral &node) override { call(node); }
//   void operator()(const BooleanLiteral &node) override { call(node); }
//   void operator()(const NumericLiteral &node) override { call(node); }
//   void operator()(const StringLiteral &node) override { call(node); }
//   void operator()(const RegularExpressionLiteral &node) override { call(node); }
//   void operator()(const ArrayLiteral &node) override { call(node); }
//   void operator()(const ObjectLiteral &node) override { call(node); }
//   void operator()(const MemberExpression &node) override { call(node); }
//   void operator()(const NewExpression &node) override { call(node); }
//   void operator()(const CallExpression &node) override { call(node); }
//   void operator()(const PostfixExpression &node) override { call(node); }
//   void operator()(const UnaryExpression &node) override { call(node); }
//   void operator()(const BinaryExpression &node) override { call(node); }
//   void operator()(const ConditionalExpression &node) override { call(node); }
//   void operator()(const FunctionExpression &node) override { call(node); }

//   void operator()(const Block &node) override { call(node); }
//   void operator()(const VariableStatement &node) override { call(node); }
//   void operator()(const EmptyStatement &node) override { call(node); }
//   void operator()(const ExpressionStatement &node) override { call(node); }
//   void operator()(const IfStatement &node) override { call(node); }
//   void operator()(const DoWhileStatement &node) override { call(node); }
//   void operator()(const WhileStatement &node) override { call(node); }
//   void operator()(const ForStatement &node) override { call(node); }
//   void operator()(const ForInStatement &node) override { call(node); }
//   void operator()(const ContinueStatement &node) override { call(node); }
//   void operator()(const BreakStatement &node) override { call(node); }
//   void operator()(const ReturnStatement &node) override { call(node); }
//   void operator()(const WithStatement &node) override { call(node); }
//   void operator()(const LabelledStatement &node) override { call(node); }
//   void operator()(const SwitchStatement &node) override { call(node); }
//   void operator()(const ThrowStatement &node) override { call(node); }
//   void operator()(const TryStatement &node) override { call(node); }
//   void operator()(const DebuggerStatement &node) override { call(node); }

//   void operator()(const CaseClause &node) override { call(node); }
//   void operator()(const DefaultClause &node) override { call(node); }

//   void operator()(const FunctionDeclaration &node) override { call(node); }
//   void operator()(const FunctionBody &node) override { call(node); }

//   void operator()(const VariableDeclaration &node) override { call(node); }
//   void operator()(const Elision &node) override { call(node); }
//   void operator()(const PropertyName &node) override { call(node); }
//   void operator()(const PropertyAssignment &node) override { call(node); }
//   void operator()(const Arguments &node) override { call(node); }
//   void operator()(const Program &node) override { call(node); }

//   void operator()(const ElementList &node) override { call(node); }
//   void operator()(const PropertyNameAndValueList &node) override { call(node); }
//   void operator()(const ArgumentList &node) override { call(node); }
//   void operator()(const VariableDeclarationList &node) override { call(node); }
//   void operator()(const StatementList &node) override { call(node); }
//   void operator()(const CaseBlock &node) override { call(node); }
//   void operator()(const SourceElements &node) override { call(node); }
//   void operator()(const FormalParameterList &node) override { call(node); }
// };

// template <typename F>
// CallableVisitor<F> make_visitor(F callback)
// {
//   return CallableVisitor<F>{callback};
// }

// template <typename... Ts>
// struct make_void {
//   typedef void type;
// };
// template <typename... Ts>
// using void_t = typename make_void<Ts...>::type;

// namespace detail {

// template <class, class, class = void_t<>>
// struct is_callable1 : std::false_type {
// };

// template <class T, class F>
// struct is_callable1<T, F,
//                     void_t<decltype(std::declval<F>()(std::declval<T>()))>>
//     : std::true_type {
// };

// template <class, class, class, class = void_t<>>
// struct is_callable2 : std::false_type {
// };

// template <class T, class N, class F>
// struct is_callable2<
//     T, N, F,
//     void_t<decltype(std::declval<F>()(std::declval<T>(), std::declval<N>()))>>
//     : std::true_type {
// };

// template <typename T, typename N, typename F>
// auto apply(T &&value, N &&next, F &&callback)
// {
//   return apply(std::forward<T>(value), std::forward<N>(next),
//                std::forward<F>(callback), is_callable2<T, N, F>{},
//                is_callable1<T, F>{});
// }

// template <typename T, typename N, typename F>
// void apply(T &&value, N &&next, F &&callback, std::true_type, std::false_type)
// {
//   callback(value, next);
// }

// template <typename T, typename N, typename F>
// void apply(T &&value, N &&, F &&callback, std::false_type, std::true_type)
// {
//   callback(value);
// }

// template <typename T, typename N, typename F>
// void apply(T &&value, N &&, F &&callback, std::false_type, std::false_type)
// {
//   // std::cout << "Not callable: " << demangle(value) << " using " <<
//   // demangle(callback) << std::endl;
// }

// } // namespace detail

// template <typename T, typename... Args>
// void accept(T &&node, Args &&... args)
// {
//   auto visitor = make_visitor([&](auto node, auto next) {
//     (void)(int[]){(detail::apply(node, next, std::forward<Args>(args)), 0)...};
//   });

//   node.accept(visitor);
// }

// struct indentstream {
//   std::ostream &           out;
//   std::string              m_indent = "";
//   std::vector<std::string> indent_stack;
//   bool                     m_skip = false;

//   void indent(const std::string &str = "  ")
//   {
//     indent_stack.push_back(m_indent);
//     m_indent += str;
//   }

//   void unindent()
//   {
//     m_indent = indent_stack.back();
//     indent_stack.pop_back();
//   }

//   void skip() { m_skip = true; }

//   indentstream &operator<<(indentstream &(*op)(indentstream &))
//   {
//     return op(*this);
//   }

//   template <typename T>
//   indentstream &operator<<(T &&what)
//   {
//     if (!m_skip) out << m_indent;
//     out << std::forward<T>(what);
//     m_skip = false;
//     return *this;
//   }
// };

// static indentstream &indent(indentstream &out)
// {
//   out.indent();
//   return out;
// }

// static indentstream &unindent(indentstream &out)
// {
//   out.unindent();
//   return out;
// }

// static indentstream &skip(indentstream &out)
// {
//   out.skip();
//   return out;
// }

// std::string to_string(const Program &program)
// {
//   using namespace std;
//   stringstream buf;
//   indentstream out{buf};

//   accept(program,
//          [&](const Program &program, auto next) {
//            out << "type: Script\n"
//                << "body:\n"
//                << indent << "type: FunctionBody\n"
//                << "directives: []\n"
//                << "statements:\n"
//                << indent;

//            for (auto stmt : *program.body) {
//              out << "- " << indent << skip;
//              next(stmt);
//              out << unindent;
//            }
//            out << unindent << unindent;

//          },
//          [&](const VariableStatement &stmt, auto next) {
//            out << "type: VariableDeclarationStatement\n"
//                << "declaration:\n"
//                << indent;
//            next(stmt.declarations);
//            out << unindent;
//            //  out << "var";
//            //  out.indent();
//            //  next(stmt.declarations);
//            //  out.unindent();
//          },
//          [&](const ExpressionStatement &stmt, auto next) {
//            //  out << "ExpressionStatement";
//            //  out.indent();
//            //  next(stmt.expression);
//            //  out.unindent();
//          },
//          [&](const BinaryExpression &expr, auto next) {
//            //  out << expr.op;
//            //  out.indent();
//            //  next(expr.lhs);
//            //  next(expr.rhs);
//            //  out.unindent();
//          },
//          [&](const Identifier &identifier) {
//            out << identifier.to_string() << "\n";
//          },
//          [&](const NumericLiteral &literal) { out << literal.value << "\n"; },
//          [&](const StringLiteral &literal) {
//            out << "\"" + literal.to_string() + "\"\n";
//          },
//          [&](const VariableDeclaration &decl, auto next) {
//            next(decl.identifier);
//            next(decl.initializer);
//          },
//          [&](auto x) { cout << "catch all: " << demangle(x) << endl; });
//   return buf.str();
// }
