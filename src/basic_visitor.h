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
    (void)(int[]){(apply_impl(std::forward<Args>(args)), 0)...};
    // https://twitter.com/SeanParent/status/558330478541803522
  }

  void operator()(const This&) {}
  void operator()(const Identifier&) {}
  void operator()(const NullLiteral&) {}
  void operator()(const BooleanLiteral&) {}
  void operator()(const NumericLiteral&) {}
  void operator()(const StringLiteral&) {}
  void operator()(const RegularExpressionLiteral&) {}

  void operator()(const ArrayLiteral& node) { apply(node.elements); }
  void operator()(const ObjectLiteral& node) { apply(node.declarations); }

  void operator()(const ThisExpression&) {}
  void operator()(const IdentifierExpression& node) { apply(node.identifier); }
  void operator()(const LiteralExpression& node) { apply(node.literal); }
  void operator()(const ArrayExpression& node) { apply(node.array); }
  void operator()(const ObjectExpression& node) { apply(node.object); }
  void operator()(const MemberExpression& node)
  {
    apply(node.object, node.property);
  }
  void operator()(const NewExpression& node)
  {
    apply(node.callee, node.arguments);
  }
  void operator()(const CallExpression& node)
  {
    apply(node.callee, node.arguments);
  }
  void operator()(const PostfixExpression& node) { apply(node.lhs); }
  void operator()(const UnaryExpression& node) { apply(node.rhs); }
  void operator()(const BinaryExpression& node) { apply(node.lhs, node.rhs); }
  void operator()(const ConditionalExpression& node)
  {
    apply(node.test, node.consequent, node.alternate);
  }
  void operator()(const FunctionExpression& node)
  {
    apply(node.id, node.params, node.body);
  }

  void operator()(const Block& node) { apply(node.body); }
  void operator()(const VariableStatement& stmt) { apply(stmt.declarations); }
  void                                     operator()(const EmptyStatement&) {}
  void operator()(const ExpressionStatement& stmt) { apply(stmt.expression); }
  void operator()(const IfStatement& stmt)
  {
    apply(stmt.test, stmt.consequent, stmt.alternate);
  }
  void operator()(const DoWhileStatement&) {}
  void operator()(const WhileStatement&) {}
  void operator()(const ForStatement&) {}
  void operator()(const ForInStatement&) {}
  void operator()(const ContinueStatement& stmt) { apply(stmt.label); }
  void operator()(const BreakStatement& stmt) { apply(stmt.label); }
  void operator()(const ReturnStatement& stmt) { apply(stmt.argument); }
  void operator()(const WithStatement& stmt) { apply(stmt.object, stmt.body); }
  void operator()(const LabelledStatement& stmt)
  {
    apply(stmt.label, stmt.body);
  }
  void operator()(const SwitchStatement& stmt)
  {
    apply(stmt.discriminant, stmt.cases);
  }
  void operator()(const ThrowStatement& stmt) { apply(stmt.argument); }
  void operator()(const TryStatement& stmt)
  {
    apply(stmt.block, stmt.handler, stmt.finalizer);
  }
  void operator()(const DebuggerStatement&) {}

  void operator()(const CaseClause& node) { apply(node.test, node.consequent); }
  void operator()(const DefaultClause& node) { apply(node.consequent); }

  void operator()(const FunctionDeclaration& decl)
  {
    apply(decl.id, decl.params, decl.body);
  }
  void operator()(const FunctionBody& node) { apply(node.data); }

  void operator()(const VariableDeclaration& decl)
  {
    apply(decl.identifier, decl.initializer);
  }
  void operator()(const Elision&) {}
  void operator()(const PropertyName&) {}
  void operator()(const PropertyAssignment& node)
  {
    apply(node.name, node.value);
  }
  void operator()(const Arguments& node) { apply(node.list); }
  void operator()(const Program& node) { apply(node.body); }
  void operator()(const ProgramDeclaration& decl) { apply(decl.body); }

  void operator()(const ElementList& list) { apply(list.data); }
  void operator()(const PropertyNameAndValueList& list) { apply(list.data); }
  void operator()(const ArgumentList& list) { apply(list.data); }
  void operator()(const VariableDeclarationList& list) { apply(list.data); }
  void operator()(const StatementList& list) { apply(list.data); }
  void operator()(const CaseBlock& list) { apply(list.data); }
  void operator()(const SourceElements& list) { apply(list.data); }
  void operator()(const FormalParameterList& list) { apply(list.data); }
};

#endif