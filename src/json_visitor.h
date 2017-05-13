#ifndef ECMASCRIPT_JSON_VISITOR_H
#define ECMASCRIPT_JSON_VISITOR_H

#include "basic_visitor.h"
#include "util.h"

#include <numeric>
#include <sstream>

#include <boost/algorithm/string.hpp>

class JSONVisitor : public BasicVisitor {
  std::stringstream buf;

  static char to_hex_digit(int x)
  {
    switch (x) {
    case 0: return '0';
    case 1: return '1';
    case 2: return '2';
    case 3: return '3';
    case 4: return '4';
    case 5: return '5';
    case 6: return '6';
    case 7: return '7';
    case 8: return '8';
    case 9: return '9';
    case 10: return 'a';
    case 11: return 'b';
    case 12: return 'c';
    case 13: return 'd';
    case 14: return 'e';
    case 15: return 'f';
    default: return '?';
    }
  }

  static std::string& encode(std::string& str, int cp)
  {
    switch (cp) {
    case '\"': str += "\\\""; break;
    case '\\': str += "\\\\"; break;
    case '/': str += "\\/"; break;
    case '\b': str += "\\b"; break;
    case '\f': str += "\\f"; break;
    case '\n': str += "\\n"; break;
    case '\r': str += "\\r"; break;
    case '\t': str += "\\t"; break;
    default:
      if (cp >= 0 && cp <= 0x0F) {
        str += "\\u000";
        str += to_hex_digit(cp & 0x0F);
      }
      else if (cp >= 0x10 && cp <= 0x1F) {
        str += "\\u00";
        str += to_hex_digit((cp / 16) & 0x0F);
        str += to_hex_digit(cp & 0x0F);
      }
      else
        str += cp;
      break;
    }
    return str;
  }

  static std::string quote(std::string str)
  {
    using std::string;
    return accumulate(str.begin(), str.end(), string("\""), encode) + "\"";
  }

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

  void operator()(const ArrayLiteral& literal) override
  {
    apply(literal.elements);
  }

  void operator()(const ObjectLiteral& literal) override
  {
    apply(literal.declarations);
  }

  void operator()(const ThisExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("ThisExpression");
    buf << "}";
  }

  void operator()(const ArrayExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("ArrayExpression");
    buf << "," << quote("elements") << ":";
    apply(expr.array);
    buf << "}";
  }

  void operator()(const ObjectExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("ObjectExpression");
    buf << "," << quote("properties") << ":";
    apply(expr.object);
    buf << "}";
  }

  void operator()(const PropertyName& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("PropertyName");
    if (expr.identifier) {
      buf << "," << quote("kind") << ":" << quote("identifier");
      buf << "," << quote("value") << ":"
          << quote(convert_utf16_to_utf8(expr.identifier->value));
    }
    else
      buf << quote("todo");
    buf << "}";
  }

  void operator()(const PropertyAssignment& expr) override
  {
    switch (expr.kind) {
    case PropertyAssignment::Kind::INIT:
      buf << "{" << quote("type") << ":" << quote("DataProperty");
      buf << "," << quote("name") << ":";
      apply(expr.name);
      buf << "," << quote("expression") << ":";
      apply(expr.expression);
      buf << "}";
      break;
    case PropertyAssignment::Kind::GET: break;
    case PropertyAssignment::Kind::SET: break;
    }
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
    buf << "," << quote("operand") << ":";
    apply(expr.rhs);
    buf << "," << quote("operator") << ":" << quote(expr.op);
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

  void operator()(const AssignmentExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("AssignmentExpression");
    buf << "," << quote("operator") << ":" << quote(expr.op);
    buf << "," << quote("binding") << ":";
    apply(expr.lhs);
    buf << "," << quote("expression") << ":";
    apply(expr.rhs);
    buf << "}";
  }

  void operator()(const MemberExpression& expr) override
  {
    buf << "{" << quote("type") << ":";

    if (expr.property)
      buf << quote("StaticMemberExpression");
    else
      buf << quote("ComputedMemberExpression");
    buf << "," << quote("object") << ":";
    apply(expr.object);
    if (expr.property) {
      buf << "," << quote("property") << ":";
      apply(expr.property);
    }
    else {
      buf << "," << quote("expression") << ":";
      apply(expr.expression);
    }
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

  void operator()(const FunctionExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("FunctionExpression");
    buf << "," << quote("name") << ":";
    if (expr.id)
      apply(expr.id);
    else
      buf << "null";
    buf << "," << quote("parameters") << ":";
    apply(expr.params);
    buf << "," << quote("body") << ":";
    apply(expr.body);
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
  void operator()(const StatementList& list) override { apply(list.data); }

  void operator()(const Block& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("BlockStatement");
    buf << "," << quote("block") << ":";
    buf << "{" << quote("type") << ":" << quote("Block");
    buf << "," << quote("statements") << ":";
    apply(stmt.body);
    buf << "}";
    buf << "}";
  }

  void operator()(const VariableStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("VariableDeclarationStatement");
    buf << "," << quote("declaration") << ":";
    apply(stmt.declarations);
    buf << "}";
  }

  void operator()(const VariableDeclarationList& list) override
  {
    buf << "{" << quote("type") << ":" << quote("VariableDeclaration");
    buf << "," << quote("kind") << ":" << quote("var");
    buf << "," << quote("declarators") << ":";
    apply(list.begin(), list.end());
    buf << "}";
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
    if (stmt.alternate)
      apply(stmt.alternate);
    else
      buf << "null";
    buf << "}";
  }

  void operator()(const ForStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("ForStatement");
    buf << "," << quote("body") << ":";
    apply(stmt.body);
    buf << "," << quote("init") << ":";
    if (stmt.init)
      apply(stmt.init);
    else
      buf << "null";
    buf << "," << quote("test") << ":";
    if (stmt.test)
      apply(stmt.test);
    else
      buf << "null";
    buf << "," << quote("update") << ":";
    if (stmt.update)
      apply(stmt.update);
    else
      buf << "null";
    buf << "}";
  }

  void operator()(const ReturnStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("ReturnStatement");
    buf << "," << quote("expression") << ":";
    if (stmt.argument) {
      apply(stmt.argument);
    }
    else
      buf << "null";
    buf << "}";
  }

  void operator()(const TryStatement& stmt) override
  {
    auto type = stmt.finalizer ? "TryFinallyStatement" : "TryCatchStatement";
    buf << "{" << quote("type") << ":" << quote(type);
    buf << "," << quote("body") << ":";

    buf << "{" << quote("type") << ":" << quote("Block");
    buf << "," << quote("statements") << ":";
    apply(stmt.block->body);
    buf << "}";

    buf << "," << quote("catchClause") << ":";
    if (stmt.handler) {
      buf << "{" << quote("type") << ":" << quote("CatchClause");
      buf << "," << quote("binding") << ":";
      apply(stmt.binding);
      buf << "," << quote("body") << ":";

      buf << "{" << quote("type") << ":" << quote("Block");
      buf << "," << quote("statements") << ":";
      apply(stmt.handler->body);
      buf << "}";

      buf << "}";
    }
    else
      buf << "null";
    if (stmt.finalizer) {
      buf << "," << quote("finalizer") << ":";
      buf << "{" << quote("type") << ":" << quote("Block");
      buf << "," << quote("statements") << ":";
      apply(stmt.finalizer->body);
      buf << "}";
    }
    buf << "}";
  }

  using BasicVisitor::apply;

  template <typename It>
  void apply(It it, It end)
  {
    buf << "[";
    if (it != end) apply(*it++);
    while (it != end) {
      buf << ",";
      apply(*it++);
    }
    buf << "]";
  }

  template <typename T>
  void apply(const std::vector<T>& list)
  {
    apply(list.begin(), list.end());
  }

  void operator()(const SourceElements& list) override { apply(list.data); }

  void operator()(const ProgramDeclaration& decl) override
  {
    buf << "{" << quote("type") << ":" << quote("Script");
    buf << "," << quote("body") << ":";
    buf << "{" << quote("type") << ":" << quote("FunctionBody");
    buf << "," << quote("directives") << ":"
        << "[]";
    buf << "," << quote("statements") << ":";
    apply(decl.body);
    buf << "}";
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

  void operator()(const ElementList& list) override
  {
    apply(list.begin(), list.end());
  }

  void operator()(const PropertyNameAndValueList& list) override
  {
    apply(list.begin(), list.end());
  }

  void operator()(const FormalParameterList& list) override
  {
    apply(list.begin(), list.end());
  }

public:
  std::string str() const { return buf.str(); }
};

#endif