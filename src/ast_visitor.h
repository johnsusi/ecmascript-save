#ifndef ECMASCRIPT_AST_VISITOR_H
#define ECMASCRIPT_AST_VISITOR_H

#include "basic_visitor.h"
#include "util.h"

#include <sstream>

class SimplifiedYAMLVisitor : public BasicVisitor {

  struct stream_t {
    std::string       m_indent;
    std::stringstream buf;
    Visitor*          visitor;
    bool              m_quote = false;

    template <typename T>
    stream_t& print(const T& value)
    {
      buf << m_indent;
      if (m_quote)
        buf << "\"";
      buf << value;
      if (m_quote)
        buf << "\"";
      buf << "\n";
      m_quote = false;
      return *this;
    }

    stream_t& operator<<(bool value)
    {
      return print(value ? "true" : "false");
    }

    stream_t& operator<<(const char* value)
    {
      return print(value);
    }

    stream_t& operator<<(const std::string& value)
    {
      return print(value);
    }

    stream_t& operator<<(double value)
    {
      return print(value);
    }

    stream_t& operator<<(const std::u16string& value)
    {
      return print(convert_utf16_to_utf8(value));
    }

    stream_t& operator<<(Node* node)
    {
      if (node)
        node->accept(*visitor);
      return *this;
    }

    stream_t& operator<<(stream_t& (*modifier)(stream_t&))
    {
      return modifier(*this);
    }

    stream_t(Visitor* visitor) : visitor(visitor)
    {
    }

  } buf = stream_t(this);

  static stream_t& indent(stream_t& out)
  {
    out.m_indent += "  ";
    return out;
  }

  static stream_t& unindent(stream_t& out)
  {
    out.m_indent.resize(out.m_indent.size() - 2);
    return out;
  }

  static stream_t& quote(stream_t& out)
  {
    out.m_quote = true;
    return out;
  }

  // void operator()(const Identifier& id) override
  // {
  //   buf << id.value;
  // }

  // void operator()(const NullLiteral& literal) override
  // {
  //   buf << "null";
  // }

  // void operator()(const BooleanLiteral& literal) override
  // {
  //   buf << literal.value;
  // }

  // void operator()(const NumericLiteral& literal) override
  // {
  //   buf << literal.value;
  // }

  // void operator()(const StringLiteral& literal) override
  // {
  //   // buf << quote << static_cast<std::string>(literal.value);
  // }

  void operator()(const PostfixExpression& expr) override
  {
    buf << expr.op << indent << expr.lhs << unindent;
  }

  void operator()(const UnaryExpression& expr) override
  {
    buf << expr.op << indent << expr.rhs << unindent;
  }

  void operator()(const BinaryExpression& expr) override
  {
    buf << expr.op.to_string() << indent << expr.lhs << expr.rhs << unindent;
  }

  void operator()(const CallExpression& expr) override
  {
    buf << "()" << indent << expr.callee << expr.arguments << unindent;
  }

  void operator()(const NewExpression& expr) override
  {
    buf << "new" << indent << expr.callee << expr.arguments << unindent;
  }

  void operator()(const MemberExpression& expr) override
  {
    buf << "." << indent << expr.object << expr.property << unindent;
  }

  void operator()(const Block& stmt) override
  {
    buf << "Block" << indent << stmt.body << unindent;
  }

  void operator()(const EmptyStatement&) override
  {
    buf << "EmptyStatement";
  }

  void operator()(const ExpressionStatement& stmt) override
  {
    buf << "ExpressionStatement" << indent << stmt.expression << unindent;
  }

  void operator()(const IfStatement& stmt) override
  {
    buf << "IfStatement" << indent << stmt.test << stmt.consequent
        << stmt.alternate << unindent;
  }

  void operator()(const ForStatement& stmt) override
  {
    buf << "ForStatement" << indent;
    if (stmt.init)
      buf << stmt.init;
    else
      buf << "null";
    if (stmt.test)
      buf << stmt.test;
    else
      buf << "null";
    if (stmt.update)
      buf << stmt.update;
    else
      buf << "null";
    buf << stmt.body << unindent;
  }

  void operator()(const ContinueStatement& stmt) override
  {
    buf << "ContinueStatement" << indent << stmt.label << unindent;
  }

  void operator()(const BreakStatement& stmt) override
  {
    buf << "BreakStatement" << indent << stmt.label << unindent;
  }

  void operator()(const ReturnStatement& stmt) override
  {
    buf << "ReturnStatement" << indent << stmt.argument << unindent;
  }

  void operator()(const LabelledStatement& stmt) override
  {
    buf << "LabelledStatement" << indent << stmt.label << stmt.body << unindent;
  }

  void operator()(const Program& program) override
  {
    buf << "Program" << indent << program.body << unindent;
  }

public:
  std::string str() const
  {
    return buf.buf.str();
  }
};

#endif