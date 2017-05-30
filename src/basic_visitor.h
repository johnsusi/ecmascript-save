#ifndef ECMASCRIPT_BASIC_VISITOR_H
#define ECMASCRIPT_BASIC_VISITOR_H

#include "ast.h"

struct BasicVisitor : Visitor {

  template <typename T>
  void apply_impl(const T* node)
  {
    if (node) node->accept(*this);
  }

  template <typename T>
  void apply_impl(const std::vector<T>& list)
  {
    for (auto child : list) apply(child);
  }

  template <typename... Args>
  void apply(Args&&... args)
  {
    using t = int[sizeof...(Args)];
    (void)t{(apply_impl(std::forward<Args>(args)), 0)...};
    // (void)(int[]){(apply_impl(std::forward<Args>(args)), 0)...};
    // https://twitter.com/SeanParent/status/558330478541803522
  }

  void operator()(const This&) override {}
  void operator()(const Identifier&) override {}
  void operator()(const NullLiteral&) override {}
  void operator()(const BooleanLiteral&) override {}
  void operator()(const NumericLiteral&) override {}
  void operator()(const StringLiteral&) override {}
  void operator()(const RegularExpressionLiteral&) override {}

  void operator()(const ArrayLiteral& node) override { apply(node.elements); }
  void operator()(const ObjectLiteral& node) override
  {
    apply(node.declarations);
  }

  void operator()(const ThisExpression&) override {}
  void operator()(const IdentifierExpression& node) override
  {
    apply(node.identifier);
  }
  void operator()(const LiteralExpression& node) override
  {
    apply(node.literal);
  }
  void operator()(const ArrayExpression& node) override { apply(node.array); }
  void operator()(const ObjectExpression& node) override { apply(node.object); }
  void operator()(const MemberExpression& node) override
  {
    apply(node.object, node.property);
  }
  void operator()(const NewExpression& node) override
  {
    apply(node.callee, node.arguments);
  }
  void operator()(const CallExpression& node) override
  {
    apply(node.callee, node.arguments);
  }
  void operator()(const PostfixExpression& node) override { apply(node.lhs); }
  void operator()(const UnaryExpression& node) override { apply(node.rhs); }
  void operator()(const BinaryExpression& node) override
  {
    apply(node.lhs, node.rhs);
  }
  void operator()(const ConditionalExpression& node) override
  {
    apply(node.test, node.consequent, node.alternate);
  }

  void operator()(const AssignmentExpression& node) override
  {
    (*this)(static_cast<const BinaryExpression&>(node));
  }

  void operator()(const FunctionExpression& node) override
  {
    apply(node.id, node.params, node.body);
  }

  void operator()(const Block& node) override { apply(node.body); }
  void operator()(const VariableStatement& stmt) override
  {
    apply(stmt.declarations);
  }
  void operator()(const EmptyStatement&) override {}
  void operator()(const ExpressionStatement& stmt) override
  {
    apply(stmt.expression);
  }
  void operator()(const IfStatement& stmt) override
  {
    apply(stmt.test, stmt.consequent, stmt.alternate);
  }
  void operator()(const DoWhileStatement&) override {}
  void operator()(const WhileStatement&) override {}
  void operator()(const ForStatement&) override {}
  void operator()(const ForInStatement&) override {}
  void operator()(const ContinueStatement& stmt) override { apply(stmt.label); }
  void operator()(const BreakStatement& stmt) override { apply(stmt.label); }
  void operator()(const ReturnStatement& stmt) override
  {
    apply(stmt.argument);
  }
  void operator()(const WithStatement& stmt) override
  {
    apply(stmt.object, stmt.body);
  }
  void operator()(const LabelledStatement& stmt) override
  {
    apply(stmt.label, stmt.body);
  }
  void operator()(const SwitchStatement& stmt) override
  {
    apply(stmt.discriminant, stmt.cases);
  }
  void operator()(const ThrowStatement& stmt) override { apply(stmt.argument); }
  void operator()(const TryStatement& stmt) override
  {
    apply(stmt.block, stmt.handler, stmt.finalizer);
  }
  void operator()(const DebuggerStatement&) override {}

  void operator()(const CaseClause& node) override
  {
    apply(node.test, node.consequent);
  }
  void operator()(const DefaultClause& node) override
  {
    apply(node.consequent);
  }

  void operator()(const FunctionDeclaration& decl) override
  {
    apply(decl.id, decl.params, decl.body);
  }
  void operator()(const FunctionBody& node) override { apply(node.data); }

  void operator()(const VariableDeclaration& decl) override
  {
    apply(decl.identifier, decl.initializer);
  }
  void operator()(const Elision&) override {}
  void operator()(const PropertyName&) override {}
  void operator()(const PropertyAssignment& node) override
  {
    switch (node.kind) {
    case PropertyAssignment::Kind::INIT:
      apply(node.name, node.expression);
      break;
    case PropertyAssignment::Kind::GET: apply(node.name, node.body); break;
    case PropertyAssignment::Kind::SET:
      apply(node.name, node.parameter, node.body);
      break;
    }
  }
  void operator()(const Arguments& node) override { apply(node.list); }
  void operator()(const Program& node) override { apply(node.body); }
  void operator()(const ProgramDeclaration& decl) override { apply(decl.body); }

  void operator()(const ElementList& list) override { apply(list.data); }
  void operator()(const PropertyNameAndValueList& list) override
  {
    apply(list.data);
  }
  void operator()(const ArgumentList& list) override { apply(list.data); }
  void operator()(const VariableDeclarationList& list) override
  {
    apply(list.data);
  }
  void operator()(const StatementList& list) override { apply(list.data); }
  void operator()(const CaseBlock& list) override { apply(list.data); }
  void operator()(const SourceElements& list) override { apply(list.data); }
  void operator()(const FormalParameterList& list) override
  {
    apply(list.data);
  }
};

#endif