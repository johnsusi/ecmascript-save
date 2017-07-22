#ifndef ECMASCRIPT_VISITOR_H
#define ECMASCRIPT_VISITOR_H

#include <memory>

struct Expression;
struct This;
struct Identifier;
struct NullLiteral;
struct BooleanLiteral;
struct NumericLiteral;
struct StringLiteral;
struct RegularExpressionLiteral;
struct PropertyName;
struct PropertyAssignment;
struct Elision;
struct ElementList;
struct ArrayLiteral;
struct ObjectLiteral;

struct ThisExpression;
struct IdentifierExpression;
struct LiteralExpression;
struct ArrayExpression;
struct ObjectExpression;

struct MemberExpression;
struct ArgumentList;
struct Arguments;
struct NewExpression;
struct CallExpression;
struct PostfixExpression;
struct UnaryExpression;
struct BinaryExpression;
struct ConditionalExpression;
struct AssignmentExpression;

struct VariableDeclaration;
struct VariableDeclarationList;
struct SourceElement;
struct Statement;
struct StatementList;
struct Block;
struct VariableStatement;
struct EmptyStatement;
struct ExpressionStatement;
struct IfStatement;
struct DoWhileStatement;
struct WhileStatement;
struct ForStatement;
struct ForInStatement;
struct ContinueStatement;
struct BreakStatement;
struct ReturnStatement;
struct WithStatement;
struct LabelledStatement;
struct CaseClause;
struct DefaultClause;
struct CaseBlock;
struct SwitchStatement;
struct ThrowStatement;
struct TryStatement;
struct DebuggerStatement;
struct Declaration;
struct SourceElements;
struct FormalParameterList;
struct FunctionBody;
struct FunctionDeclaration;
struct FunctionExpression;
struct Program;

struct ElementList;
struct PropertyNameAndValueList;
struct ArgumentList;
struct VariableDeclarationList;
struct StatementList;
struct CaseBlock;
struct SourceElements;
struct FormalParameterList;
struct ProgramDeclaration;

struct Visitor {

  // virtual void operator()(const This&)                     = 0;
  // virtual void operator()(const Identifier&)               = 0;
  // virtual void operator()(const NullLiteral&)              = 0;
  // virtual void operator()(const BooleanLiteral&)           = 0;
  // virtual void operator()(const NumericLiteral&)           = 0;
  // virtual void operator()(const StringLiteral&)            = 0;
  // virtual void operator()(const RegularExpressionLiteral&) = 0;
  // virtual void operator()(const ArrayLiteral&)             = 0;
  // virtual void operator()(const ObjectLiteral&)            = 0;

  virtual void operator()(const ThisExpression&)        = 0;
  virtual void operator()(const IdentifierExpression&)  = 0;
  virtual void operator()(const LiteralExpression&)     = 0;
  virtual void operator()(const ArrayExpression&)       = 0;
  virtual void operator()(const ObjectExpression&)      = 0;
  virtual void operator()(const MemberExpression&)      = 0;
  virtual void operator()(const NewExpression&)         = 0;
  virtual void operator()(const CallExpression&)        = 0;
  virtual void operator()(const PostfixExpression&)     = 0;
  virtual void operator()(const UnaryExpression&)       = 0;
  virtual void operator()(const BinaryExpression&)      = 0;
  virtual void operator()(const ConditionalExpression&) = 0;
  virtual void operator()(const AssignmentExpression&)  = 0;
  virtual void operator()(const FunctionExpression&)    = 0;

  virtual void operator()(const Block&)               = 0;
  virtual void operator()(const VariableStatement&)   = 0;
  virtual void operator()(const EmptyStatement&)      = 0;
  virtual void operator()(const ExpressionStatement&) = 0;
  virtual void operator()(const IfStatement&)         = 0;
  virtual void operator()(const DoWhileStatement&)    = 0;
  virtual void operator()(const WhileStatement&)      = 0;
  virtual void operator()(const ForStatement&)        = 0;
  virtual void operator()(const ForInStatement&)      = 0;
  virtual void operator()(const ContinueStatement&)   = 0;
  virtual void operator()(const BreakStatement&)      = 0;
  virtual void operator()(const ReturnStatement&)     = 0;
  virtual void operator()(const WithStatement&)       = 0;
  virtual void operator()(const LabelledStatement&)   = 0;
  virtual void operator()(const SwitchStatement&)     = 0;
  virtual void operator()(const ThrowStatement&)      = 0;
  virtual void operator()(const TryStatement&)        = 0;
  virtual void operator()(const DebuggerStatement&)   = 0;

  virtual void operator()(const CaseClause&)    = 0;
  virtual void operator()(const DefaultClause&) = 0;

  virtual void operator()(const FunctionDeclaration&) = 0;
  virtual void operator()(const FunctionBody&)        = 0;

  virtual void operator()(const VariableDeclaration&) = 0;
  virtual void operator()(const Elision&)             = 0;
  virtual void operator()(const PropertyName&)        = 0;
  virtual void operator()(const PropertyAssignment&)  = 0;
  virtual void operator()(const Arguments&)           = 0;
  virtual void operator()(const Program&)             = 0;

  virtual void operator()(const ElementList&)              = 0;
  virtual void operator()(const PropertyNameAndValueList&) = 0;
  virtual void operator()(const ArgumentList&)             = 0;
  virtual void operator()(const VariableDeclarationList&)  = 0;
  virtual void operator()(const StatementList&)            = 0;
  virtual void operator()(const CaseBlock&)                = 0;
  virtual void operator()(const SourceElements&)           = 0;
  virtual void operator()(const FormalParameterList&)      = 0;
  virtual void operator()(const ProgramDeclaration&)       = 0;
};

#endif