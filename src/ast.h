#ifndef ECMASCRIPT_AST_H
#define ECMASCRIPT_AST_H

#include <vector>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

struct This {
};
struct Identifier {
  std::u16string value;
};
struct NullLiteral {
};
struct BooleanLiteral {
  bool value;
};
struct NumericLiteral {
  double value;
};
struct StringLiteral {
  std::u16string value;
};
struct RegularExpressionLiteral {
};
using Literal = boost::variant<NullLiteral, BooleanLiteral, NumericLiteral,
                               StringLiteral, RegularExpressionLiteral>;
struct ArrayLiteral;
struct ObjectLiteral;
using PrimaryExpression = boost::variant<This, Identifier, Literal,
                                         boost::recursive_wrapper<ArrayLiteral>,
                                         boost::recursive_wrapper<ObjectLiteral>
                                         // ( expression )
                                         >;
struct MemberExpression;
struct NewExpression;
struct CallExpression;
struct PostfixExpression;
struct UnaryExpression;
struct BinaryExpression;
struct ConditionalExpression;
struct Arguments;

using Expression =
    boost::variant<This, Identifier, boost::recursive_wrapper<ArrayLiteral>,
                   boost::recursive_wrapper<ObjectLiteral>, NullLiteral,
                   BooleanLiteral, NumericLiteral, StringLiteral,
                   RegularExpressionLiteral,
                   boost::recursive_wrapper<MemberExpression>,
                   boost::recursive_wrapper<NewExpression>,
                   boost::recursive_wrapper<CallExpression>,
                   boost::recursive_wrapper<PostfixExpression>,
                   boost::recursive_wrapper<UnaryExpression>,
                   boost::recursive_wrapper<BinaryExpression>,
                   boost::recursive_wrapper<ConditionalExpression>,
                   boost::recursive_wrapper<Arguments>>;

using PropertyName = boost::variant<
    // IdentifierName,
    StringLiteral, NumericLiteral>;

struct PropertyAssignment {
  enum class Kind { INIT, GET, SET } kind;
  PropertyName name;
  Expression value;
};

struct ArrayLiteral {
  std::vector<Expression> elements;
};

struct ObjectLiteral {
  std::vector<PropertyAssignment> declarations;
};

struct MemberExpression {
  Expression object;
  Expression property;
};

using ArgumentList = std::vector<Expression>;

struct Arguments {
  ArgumentList list;
};

struct NewExpression {
  Expression callee;
  Arguments arguments;
};

struct CallExpression {
  Expression callee;
  Arguments arguments;
};

struct PostfixExpression {
  std::string op;
  Expression lhs;
};

struct UnaryExpression {
  std::string op;
  Expression rhs;
};

struct BinaryExpression {
  std::string op;
  Expression lhs;
  Expression rhs;
};

struct ConditionalExpression {
  Expression test, consequent, alternate;
};

struct VariableDeclaration {
  Identifier identifier;
  boost::optional<Expression> initializer;
};

using VariableDeclarationList = std::vector<VariableDeclaration>;

struct Block;
struct VariableStatement;
struct EmptyStatement;
struct ExpressionStatement;
struct IfStatement;
struct DoWhileStatement;
struct WhileStatement;
struct ForStatement;
struct ForInStatement;
using IterationStatement =
    boost::variant<boost::recursive_wrapper<DoWhileStatement>,
                   boost::recursive_wrapper<WhileStatement>,
                   boost::recursive_wrapper<ForStatement>,
                   boost::recursive_wrapper<ForInStatement>>;
struct ContinueStatement;
struct BreakStatement;
struct ReturnStatement;
struct WithStatement;
struct LabelledStatement;
struct SwitchStatement;
struct ThrowStatement;
struct TryStatement;
struct DebuggerStatement;

using Statement =
    boost::variant<boost::recursive_wrapper<Block>,
                   boost::recursive_wrapper<VariableStatement>,
                   boost::recursive_wrapper<EmptyStatement>,
                   boost::recursive_wrapper<ExpressionStatement>,
                   boost::recursive_wrapper<IfStatement>,
                   // boost::recursive_wrapper<IterationStatement>,
                   boost::recursive_wrapper<DoWhileStatement>,
                   boost::recursive_wrapper<WhileStatement>,
                   boost::recursive_wrapper<ForStatement>,
                   boost::recursive_wrapper<ForInStatement>,
                   boost::recursive_wrapper<ContinueStatement>,
                   boost::recursive_wrapper<BreakStatement>,
                   boost::recursive_wrapper<ReturnStatement>,
                   boost::recursive_wrapper<WithStatement>,
                   boost::recursive_wrapper<LabelledStatement>,
                   boost::recursive_wrapper<SwitchStatement>,
                   boost::recursive_wrapper<ThrowStatement>,
                   boost::recursive_wrapper<TryStatement>,
                   boost::recursive_wrapper<DebuggerStatement>>;

using StatementList = std::vector<Statement>;

struct Block {
  StatementList body;
};

struct VariableStatement {
};

struct EmptyStatement {
};

struct ExpressionStatement {
  Expression expression;
};

struct IfStatement {
  Expression test;
  Statement consequent;
  boost::optional<Statement> alternate;
};

struct DoWhileStatement {
  Statement body;
  Expression test;
};

struct WhileStatement {
  Expression test;
  Statement body;
};

struct ForStatement {
};

struct ForInStatement {
};

struct ContinueStatement {
  boost::optional<Identifier> label;
};

struct BreakStatement {
  boost::optional<Identifier> label;
};

struct ReturnStatement {
  boost::optional<Expression> argument;
};

struct WithStatement {
  Expression object;
  Statement body;
};

struct LabelledStatement {
  Identifier label;
  Statement body;
};

struct SwitchStatement {
  Expression discriminant;
  // CaseBlock cases;
};

struct ThrowStatement {
  Expression argument;
};

struct TryStatement {
  Block block;
  boost::optional<Block> handler;
  boost::optional<Block> finalizer;
};

struct DebuggerStatement {
};

struct FunctionDeclaration;

using SourceElement =
    boost::variant<Statement, boost::recursive_wrapper<FunctionDeclaration>>;

using SourceElements = std::vector<SourceElement>;

using FormalParameterList = std::vector<Identifier>;

struct FunctionBody {
  SourceElements elements;
};

struct FunctionDeclaration {
  Identifier id;
  FormalParameterList params;
  FunctionBody body;
};

struct FunctionExpression {
  boost::optional<Identifier> id;
  FormalParameterList params;
  FunctionBody body;
};

struct Program {
  SourceElements elements;
};

std::string normalize(const std::string &);
std::string to_string(const Program &);
bool operator==(const Program &, const std::string &);

std::ostream &operator<<(std::ostream &, const Program &);

#endif