#ifndef ECMASCRIPT_JSON_VISITOR_H
#define ECMASCRIPT_JSON_VISITOR_H

#include "basic_visitor.h"
#include "util.h"

#include <sstream>

class JSONVisitor : public BasicVisitor {
  std::stringstream buf;

  std::string quote(const std::string& str) const { return "\"" + str + "\""; }

  void operator()(const Identifier& id) override
  {
    // buf << "{" << quote("type") << ":" << quote("IdentifierExpression");
    // buf << "," << quote("identifier") << ":";
    buf << "{" << quote("type") << ":" << quote("Identifier");
    buf << "," << quote("name") << ":"
        << quote(convert_utf16_to_utf8(id.value));
    buf << "}";
    // buf << "}";
  }

  void operator()(const IdentifierExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("IdentifierExpression");
    buf << "," << quote("identifier") << ":";
    apply(expr.identifier);
    buf << "}";
  }

  // void operator()(const PrimaryExpression& expr) override {}

  void operator()(const NullLiteral& literal) override
  {
    buf << "{" << quote("type") << ":" << quote("LiteralNullExpression");
    buf << "}";
  }

  void operator()(const BooleanLiteral& literal) override
  {
    buf << "{" << quote("type") << ":" << quote("LiteralBooleanExpression");
    buf << "," << quote("value") << ":" << (literal.value ? "true" : "false");
    buf << "}";
  }

  void operator()(const NumericLiteral& literal) override
  {
    buf << "{" << quote("type") << ":" << quote("LiteralNumericExpression");
    buf << "," << quote("value") << ":" << literal.value;
    buf << "}";
  }

  void operator()(const StringLiteral& literal) override
  {
    buf << "{" << quote("type") << ":" << quote("LiteralStringExpression");
    buf << "," << quote("value") << ":"
        << quote(convert_utf16_to_utf8(literal.value));
    buf << "}";
  }

  void operator()(const PostfixExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("PostfixExpression");
    buf << "," << quote("operator") << ":" << quote(expr.op);
    buf << "," << quote("operand") << ":";
    apply(expr.lhs);
    buf << "}";
  }

  void operator()(const UnaryExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("PrefixExpression");
    buf << "," << quote("operator") << ":" << quote(expr.op);
    buf << "," << quote("operand") << ":";
    apply(expr.rhs);
    buf << "}";
  }

  void operator()(const BinaryExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("BinaryExpression");
    buf << "," << quote("operator") << ":" << quote(expr.op);
    buf << "," << quote("left") << ":";
    apply(expr.lhs);
    buf << "," << quote("right") << ":";
    apply(expr.rhs);
    buf << "}";
  }

  void operator()(const CallExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("CallExpression");
    buf << "," << quote("callee") << ":";
    apply(expr.callee);
    buf << "," << quote("arguments") << ":";
    apply(expr.arguments);
    buf << "}";
  }

  void operator()(const Arguments& args) override
  {
    if (args.list)
      apply(args.list);
    else
      buf << "[]";
  }

  void operator()(const ArgumentList& list) override { apply(list.data); }

  void operator()(const Block& args) override
  {
    buf << "{" << quote("type") << ":" << quote("Block");
    buf << "}";
  }

  void operator()(const VariableStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("VariableDeclarationStatement");
    buf << "," << quote("declaration") << ":";
    buf << "{" << quote("type") << ":" << quote("VariableDeclaration");
    buf << "," << quote("kind") << ":" << quote("var");
    buf << "," << quote("declarators") << ":";
    apply(stmt.declarations);
    buf << "}";

    buf << "}";
  }

  void operator()(const VariableDeclarationList& list) override
  {
    apply(list.data);
  }

  void operator()(const VariableDeclaration& decl) override
  {
    buf << "{" << quote("type") << ":" << quote("VariableDeclarator");
    buf << "," << quote("binding") << ":";
    apply(decl.identifier);
    buf << "," << quote("init") << ":";
    if (decl.initializer)
      apply(decl.initializer);
    else
      buf << "null";
    buf << "}";
  }

  void operator()(const EmptyStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("EmptyStatement");
    buf << "}";
  }

  void operator()(const ExpressionStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("ExpressionStatement");
    buf << "," << quote("expression") << ":";
    apply(stmt.expression);
    buf << "}";
  }

  void operator()(const IfStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("IfStatement");
    buf << "," << quote("test") << ":";
    apply(stmt.test);
    buf << "," << quote("consequent") << ":";
    apply(stmt.consequent);
    buf << "," << quote("alternate") << ":";
    apply(stmt.alternate);
    buf << "}";
  }

  using BasicVisitor::apply;
  template <typename T>
  void apply(const std::vector<T>& list)
  {
    buf << "[";
    auto it = list.begin();
    if (it != list.end()) apply(*it++);
    while (it != list.end()) {
      buf << ",";
      apply(*it++);
    }
    buf << "]";
  }

  void operator()(const SourceElements& list) override { apply(list.data); }

  void operator()(const ProgramDeclaration& decl) override
  {
    buf << "{" << quote("type") << ":" << quote("Script");
    buf << "," << quote("body") << ":";
    apply(decl.body);
    buf << "}";
  }

  void operator()(const FunctionBody& body) override
  {
    buf << "{" << quote("type") << ":" << quote("FunctionBody");
    buf << "," << quote("directives") << ":"
        << "[]";
    buf << "," << quote("statements") << ":";
    apply(body.data);
    buf << "}";
  }

  void operator()(const FunctionDeclaration& decl) override
  {
    buf << "{" << quote("type") << ":" << quote("FunctionDeclaration");
    buf << "," << quote("name") << ":";
    if (decl.id)
      apply(decl.id);
    else
      buf << "null";
    buf << "," << quote("parameters") << ":";
    if (decl.params)
      apply(decl.params);
    else
      buf << "[]";
    buf << "," << quote("body") << ":";
    apply(decl.body);
    buf << "}";
  }

  void operator()(const FormalParameterList& list) override
  {
    apply(list.data);
  }

public:
  std::string str() const { return buf.str(); }
};

#endif