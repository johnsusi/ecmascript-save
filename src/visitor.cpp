#include "visitor.h"

#include <memory>

template <typename Lambda>
struct LambdaVisitor : Visitor {
  Lambda lambda;

  template <typename T>
  auto invoke(const T& node) -> decltype(lambda(node), void())
  {
    lambda(node);
  }

  void invoke(...) {}
};

struct DelegateVistor : Visitor {
  std::unique_ptr<Visitor> delegate;

  template <typename T>
  inline void invoke(const T& node)
  {
    (*delegate)(node);
  }

  void operator()(const This& node) { return invoke(node); }
  void operator()(const Identifier& node) { return invoke(node); }
  void operator()(const NullLiteral& node) { return invoke(node); }
  void operator()(const BooleanLiteral& node) { return invoke(node); }
  void operator()(const NumericLiteral& node) { return invoke(node); }
  void operator()(const StringLiteral& node) { return invoke(node); }
  void operator()(const RegularExpressionLiteral& node) { return invoke(node); }
  void operator()(const ArrayLiteral& node) { return invoke(node); }
  void operator()(const ObjectLiteral& node) { return invoke(node); }

  void operator()(const ThisExpression& node) { return invoke(node); }
  void operator()(const IdentifierExpression& node) { return invoke(node); }
  void operator()(const LiteralExpression& node) { return invoke(node); }
  void operator()(const ArrayExpression& node) { return invoke(node); }
  void operator()(const ObjectExpression& node) { return invoke(node); }
  void operator()(const MemberExpression& node) { return invoke(node); }
  void operator()(const NewExpression& node) { return invoke(node); }
  void operator()(const CallExpression& node) { return invoke(node); }
  void operator()(const PostfixExpression& node) { return invoke(node); }
  void operator()(const UnaryExpression& node) { return invoke(node); }
  void operator()(const BinaryExpression& node) { return invoke(node); }
  void operator()(const ConditionalExpression& node) { return invoke(node); }
  void operator()(const AssignmentExpression& node) { return invoke(node); }
  void operator()(const FunctionExpression& node) { return invoke(node); }

  void operator()(const Block& node) { return invoke(node); }
  void operator()(const VariableStatement& node) { return invoke(node); }
  void operator()(const EmptyStatement& node) { return invoke(node); }
  void operator()(const ExpressionStatement& node) { return invoke(node); }
  void operator()(const IfStatement& node) { return invoke(node); }
  void operator()(const DoWhileStatement& node) { return invoke(node); }
  void operator()(const WhileStatement& node) { return invoke(node); }
  void operator()(const ForStatement& node) { return invoke(node); }
  void operator()(const ForInStatement& node) { return invoke(node); }
  void operator()(const ContinueStatement& node) { return invoke(node); }
  void operator()(const BreakStatement& node) { return invoke(node); }
  void operator()(const ReturnStatement& node) { return invoke(node); }
  void operator()(const WithStatement& node) { return invoke(node); }
  void operator()(const LabelledStatement& node) { return invoke(node); }
  void operator()(const SwitchStatement& node) { return invoke(node); }
  void operator()(const ThrowStatement& node) { return invoke(node); }
  void operator()(const TryStatement& node) { return invoke(node); }
  void operator()(const DebuggerStatement& node) { return invoke(node); }

  void operator()(const CaseClause& node) { return invoke(node); }
  void operator()(const DefaultClause& node) { return invoke(node); }

  void operator()(const FunctionDeclaration& node) { return invoke(node); }
  void operator()(const FunctionBody& node) { return invoke(node); }

  void operator()(const VariableDeclaration& node) { return invoke(node); }
  void operator()(const Elision& node) { return invoke(node); }
  void operator()(const PropertyName& node) { return invoke(node); }
  void operator()(const PropertyAssignment& node) { return invoke(node); }
  void operator()(const Arguments& node) { return invoke(node); }
  void operator()(const Program& node) { return invoke(node); }

  void operator()(const ElementList& node) { return invoke(node); }
  void operator()(const PropertyNameAndValueList& node) { return invoke(node); }
  void operator()(const ArgumentList& node) { return invoke(node); }
  void operator()(const VariableDeclarationList& node) { return invoke(node); }
  void operator()(const StatementList& node) { return invoke(node); }
  void operator()(const CaseBlock& node) { return invoke(node); }
  void operator()(const SourceElements& node) { return invoke(node); }
  void operator()(const FormalParameterList& node) { return invoke(node); }
  void operator()(const ProgramDeclaration& node) { return invoke(node); }
};
