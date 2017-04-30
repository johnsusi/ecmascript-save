#ifndef ECMASCRIPT_AST_H
#define ECMASCRIPT_AST_H

#include <iostream>
#include <vector>
#include <sstream>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace ast {

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
struct MemberExpression;
struct ArgumentList;
struct Arguments;
struct NewExpression;
struct CallExpression;
struct PostfixExpression;
struct UnaryExpression;
struct BinaryExpression;
struct ConditionalExpression;
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


struct Visitor;

struct Node {
  virtual ~Node() {}
  virtual void accept(Visitor&) const = 0;
};

template <typename T> struct List : Node {
  std::vector<T *> data;

  template <typename... Args>
  List(Args &&... args) : data(std::forward<Args>(args)...)
  {
  }

  auto front() const { return data.front(); }
  auto back() const { return data.back(); }
  void pop_back() { data.pop_back(); }
  void push_back(T *value) { data.push_back(value); }
  auto begin() const { return data.begin(); }
  auto end() const { return data.end(); }
  auto operator[](std::size_t index) { return data[index]; }

  void accept(Visitor& visitor) const override;
};

struct Visitor {
  template <typename T>
  void operator()(const List<T>& list)
  {
    for (auto item : list) item->accept(*this);
  }

  virtual void operator()(const This&) = 0;
  virtual void operator()(const Identifier&) = 0;
  virtual void operator()(const NullLiteral&) = 0;
  virtual void operator()(const BooleanLiteral&) = 0;
  virtual void operator()(const NumericLiteral&) = 0;
  virtual void operator()(const StringLiteral&) = 0;
  virtual void operator()(const RegularExpressionLiteral&) = 0;
  virtual void operator()(const ArrayLiteral&) = 0;
  virtual void operator()(const ObjectLiteral&) = 0;
  virtual void operator()(const MemberExpression&) = 0;
  virtual void operator()(const NewExpression&) = 0;
  virtual void operator()(const CallExpression&) = 0;
  virtual void operator()(const PostfixExpression&) = 0;
  virtual void operator()(const UnaryExpression&) = 0;
  virtual void operator()(const BinaryExpression&) = 0;
  virtual void operator()(const ConditionalExpression&) = 0;
  virtual void operator()(const FunctionExpression&) = 0;

  virtual void operator()(const Block&) = 0;
  virtual void operator()(const VariableStatement&) = 0;
  virtual void operator()(const EmptyStatement&) = 0;
  virtual void operator()(const ExpressionStatement&) = 0;
  virtual void operator()(const IfStatement&) = 0;
  virtual void operator()(const DoWhileStatement&) = 0;
  virtual void operator()(const WhileStatement&) = 0;
  virtual void operator()(const ForStatement&) = 0;
  virtual void operator()(const ForInStatement&) = 0;
  virtual void operator()(const ContinueStatement&) = 0;
  virtual void operator()(const BreakStatement&) = 0;
  virtual void operator()(const ReturnStatement&) = 0;
  virtual void operator()(const WithStatement&) = 0;
  virtual void operator()(const LabelledStatement&) = 0;
  virtual void operator()(const SwitchStatement&) = 0;
  virtual void operator()(const ThrowStatement&) = 0;
  virtual void operator()(const TryStatement&) = 0;
  virtual void operator()(const DebuggerStatement&) = 0;

  virtual void operator()(const CaseClause&) = 0;
  virtual void operator()(const DefaultClause&) = 0;

  virtual void operator()(const FunctionDeclaration&) = 0;
  // virtual void operator()(const FunctionBody&) = 0;

  virtual void operator()(const VariableDeclaration&) = 0;
  virtual void operator()(const Elision&) = 0;
  virtual void operator()(const PropertyName&) = 0;
  virtual void operator()(const PropertyAssignment&) = 0;
  virtual void operator()(const Arguments&) = 0;

};

template <typename T> void List<T>::accept(Visitor& visitor) const
{
  return visitor(*this);
}
struct Expression : Node {
};

struct This : Expression {
  void accept(Visitor& visitor) const override { return visitor(*this); }
};
struct Identifier : Expression {
  std::u16string value;
  Identifier(std::u16string value) : value(value) {}
  void accept(Visitor& visitor) const override { return visitor(*this); }
};
struct NullLiteral : Expression {
  void accept(Visitor& visitor) const override { return visitor(*this); }
};
struct BooleanLiteral : Expression {
  bool value;
  BooleanLiteral(bool value) : value(value) {}
  void accept(Visitor& visitor) const override { return visitor(*this); }
};
struct NumericLiteral : Expression {
  double value;
  NumericLiteral(double value) : value(value) {}
  void accept(Visitor& visitor) const override { return visitor(*this); }
};
struct StringLiteral : Expression {
  std::u16string value;
  StringLiteral(std::u16string value) : value(value) {}
  StringLiteral(const std::string &value) : value(value.begin(), value.end()) {}
  void accept(Visitor& visitor) const override { return visitor(*this); }
};
struct RegularExpressionLiteral : Expression {
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct PropertyName : Expression {
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct PropertyAssignment : Expression {
  enum class Kind { INIT, GET, SET } kind;
  PropertyName *name;
  Expression *value;
  PropertyAssignment(PropertyName* name = nullptr, Expression* value = nullptr) : name(name), value(value) {}
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct Elision : Expression {
  std::size_t count = 1;
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct ElementList : List<Expression> {
};

struct ArrayLiteral : Expression {
  ElementList *elements;
  Elision *elision;
  ArrayLiteral(ElementList *elements = nullptr, Elision *elision = nullptr)
      : elements(elements), elision(elision)
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct PropertyNameAndValueList : List<PropertyAssignment>
{
};

struct ObjectLiteral : Expression {
  PropertyNameAndValueList* declarations;
  ObjectLiteral(PropertyNameAndValueList* declarations = nullptr) : declarations(declarations)
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct MemberExpression : Expression {
  Expression *object;
  Expression *property;
  MemberExpression(Expression *object, Expression *property)
      : object(object), property(property)
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct ArgumentList : List<Expression> {
  template <typename... Args>
  ArgumentList(Args &&... args) : List(std::forward<Args>(args)...)
  {
  }
};

struct Arguments : Expression {
  ArgumentList *list;
  Arguments(ArgumentList *list = nullptr) : list(list) {}
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct NewExpression : Expression {
  Expression *callee;
  Arguments *arguments;
  NewExpression(Expression *callee, Arguments *arguments = nullptr)
      : callee(callee), arguments(arguments)
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct CallExpression : Expression {
  Expression *callee;
  Arguments *arguments;
  CallExpression(Expression *callee, Arguments *arguments = nullptr)
      : callee(callee), arguments(arguments)
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct PostfixExpression : Expression {
  std::string op;
  Expression *lhs;
  PostfixExpression(std::string op, Expression *lhs = nullptr)
      : op(op), lhs(lhs)
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct UnaryExpression : Expression {
  std::string op;
  Expression *rhs;
  UnaryExpression(std::string op, Expression *rhs = nullptr) : op(op), rhs(rhs)
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct BinaryExpression : Expression {
  std::string op;
  Expression *lhs;
  Expression *rhs;
  BinaryExpression(std::string op, Expression *lhs = nullptr,
                   Expression *rhs = nullptr)
      : op(op), lhs(lhs), rhs(rhs)
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct ConditionalExpression : Expression {
  Expression *test;
  Expression *consequent;
  Expression *alternate;
  ConditionalExpression(Expression *test, Expression *consequent,
                        Expression *alternate)
      : test(test), consequent(consequent), alternate(alternate)
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct VariableDeclaration : Expression {
  Identifier *identifier;
  Expression *initializer;
  VariableDeclaration(Identifier *identifier, Expression *initializer = nullptr)
      : identifier(identifier), initializer(initializer)
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct VariableDeclarationList : List<VariableDeclaration> {
  template <typename... Args>
  VariableDeclarationList(Args &&... args) : List(std::forward<Args>(args)...)
  {
  }
};

struct SourceElement : Node {
};

struct Statement : SourceElement {
};

struct StatementList : List<Statement> {
  template <typename... Args>
  StatementList(Args &&... args) : List(std::forward<Args>(args)...)
  {
  }
};

struct Block : Statement {
  StatementList *body;
  Block(StatementList *body = nullptr) : body(body) {}
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct VariableStatement : Statement {
  VariableDeclarationList *declarations;
  VariableStatement(VariableDeclarationList *declarations)
      : declarations(declarations)
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct EmptyStatement : Statement {
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct ExpressionStatement : Statement {
  Expression *expression;
  ExpressionStatement(Expression *expression) : expression(expression) {}
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct IfStatement : Statement {
  Expression *test;
  Statement *consequent;
  Statement *alternate;
  IfStatement(Expression *test, Statement *consequent,
              Statement *alternate = nullptr)
      : test(test), consequent(consequent), alternate(alternate)
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct DoWhileStatement : Statement {
  Statement *body;
  Expression *test;
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct WhileStatement : Statement {
  Expression *test;
  Statement *body;
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct ForStatement : Statement {
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct ForInStatement : Statement {
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct ContinueStatement : Statement {
  Identifier *label;
  ContinueStatement(Identifier *label = nullptr) : label(label) {}
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct BreakStatement : Statement {
  Identifier *label;
  BreakStatement(Identifier *label = nullptr) : label(label) {}
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct ReturnStatement : Statement {
  Expression *argument;
  ReturnStatement(Expression *argument = nullptr) : argument(argument) {}
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct WithStatement : Statement {
  Expression *object;
  Statement *body;
  WithStatement(Expression *object, Statement *body)
      : object(object), body(body)
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct LabelledStatement : Statement {
  Identifier *label;
  Statement *body;
  LabelledStatement(Identifier *label, Statement *body)
      : label(label), body(body)
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct CaseClause : Node {
  Expression *test;
  StatementList *consequent;
  CaseClause(Expression *test, StatementList *consequent)
      : test(test), consequent(consequent)
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct DefaultClause : CaseClause {
  DefaultClause(StatementList *consequent) : CaseClause(nullptr, consequent) {}
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct CaseBlock : List<CaseClause> {
};

struct SwitchStatement : Statement {
  Expression *discriminant;
  CaseBlock *cases;
  SwitchStatement(Expression *discriminant, CaseBlock *cases)
      : discriminant(discriminant), cases(cases)
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct ThrowStatement : Statement {
  Expression *argument;
  ThrowStatement(Expression *argument) : argument(argument) {}
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct TryStatement : Statement {
  Block *block;
  Block *handler;
  Block *finalizer;
  TryStatement(Block *block, Block *handler = nullptr,
               Block *finalizer = nullptr)
      : block(block), handler(handler), finalizer(finalizer)
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct DebuggerStatement : Statement {
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct Declaration : SourceElement {
};

struct SourceElements : List<SourceElement> {
  template <typename... Args>
  SourceElements(Args &&... args) : List(std::forward<Args>(args)...)
  {
  }
};

struct FormalParameterList : List<Identifier> {
};

struct FunctionBody : SourceElements {
  FunctionBody(SourceElements* body = nullptr) : SourceElements(std::move(body->data))
  {
  }
  void accept(Visitor& visitor) const override { return visitor(*this); }
};

struct FunctionDeclaration : SourceElement {
  Identifier *id;
  FormalParameterList *params;
  FunctionBody *body;
  FunctionDeclaration(Identifier *id, FormalParameterList *params,
                      FunctionBody *body)
      : id(id), params(params), body(body)
  {
  }
  void accept(Visitor& visitor) const { return visitor(*this); }
};

struct FunctionExpression : Expression {
  Identifier *id;
  FormalParameterList *params;
  FunctionBody *body;
  FunctionExpression(Identifier *id, FormalParameterList *params,
                     FunctionBody *body)
      : id(id), params(params), body(body)
  {
  }
  void accept(Visitor& visitor) const { return visitor(*this); }
};

struct Program {

  std::shared_ptr<void> storage;

  SourceElements* body;

};

void accept(const Program& program, Visitor&& visitor);

} // namespace ast



std::string to_string(const ast::Program& program);

std::string normalize(const std::string &);
// std::string to_string(const ast::Program &);
// bool operator==(const ast::Program &, const std::string &);

// std::ostream &operator<<(std::ostream &, const Program &);

#endif