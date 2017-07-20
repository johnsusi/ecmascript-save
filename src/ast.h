#ifndef ECMASCRIPT_AST_H
#define ECMASCRIPT_AST_H

#include "visitor.h"

#include "types/boolean.h"
#include "types/number.h"
#include "types/string.h"

#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct Node {
  virtual ~Node()
  {
  }
  virtual void        accept(Visitor&) const = 0;
  virtual const char* type() const           = 0;
};

template <typename T>
struct List : virtual Node {
  std::vector<T*> data;

  template <typename... Args>
  List(Args&&... args) : data(std::forward<Args>(args)...)
  {
  }

  explicit List(T* arg) : data(1, arg)
  {
  }

  auto front() const
  {
    return data.front();
  }
  auto back() const
  {
    return data.back();
  }
  void pop_back()
  {
    data.pop_back();
  }
  void push_back(T* value)
  {
    data.push_back(value);
  }
  auto begin() const
  {
    return data.begin();
  }
  auto end() const
  {
    return data.end();
  }
  auto operator[](std::size_t index)
  {
    return data[index];
  }
  auto size() const
  {
    return data.size();
  }
};

struct This : Node {
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "This";
  }
};
struct Identifier : Node {
  std::u16string value;
  Identifier(std::u16string value) : value(value)
  {
  }
  std::string to_string() const
  {
    return {value.begin(), value.end()};
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "Identifier";
  }
};

struct Literal : Node {
};

struct NullLiteral : Literal {
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "NullLiteral";
  }
};
struct BooleanLiteral : Literal {
  Boolean value;
  BooleanLiteral(Boolean value) : value(value)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "BooleanLiteral";
  }
};
struct NumericLiteral : Literal {
  Number value;
  NumericLiteral(double value) : value(value)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "NumericLiteral";
  }
};
struct StringLiteral : Literal {
  String value;
  StringLiteral(String value) : value(std::move(value))
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "StringLiteral";
  }
};
struct RegularExpressionLiteral : Literal {
  std::u16string value;
  RegularExpressionLiteral(std::u16string value) : value(value)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "RegularExpressionLiteral";
  }
};

struct ArrayLiteral : Node {
  ElementList* elements;
  Elision*     elision;
  ArrayLiteral(ElementList* elements = nullptr, Elision* elision = nullptr)
      : elements(elements), elision(elision)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "ArrayLiteral";
  }
};

struct Expression : virtual Node {
};

struct LeftHandSideExpression : Expression {
};

struct PrimaryExpression : LeftHandSideExpression {
};

struct ThisExpression : PrimaryExpression {
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "ThisExpression";
  }
};

struct IdentifierExpression : PrimaryExpression {
  Identifier* identifier;
  IdentifierExpression(Identifier* identifier) : identifier(identifier)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "IdentifierExpression";
  }
};

struct LiteralExpression : PrimaryExpression {
  Literal* literal;
  LiteralExpression(Literal* literal) : literal(literal)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "LiteralExpression";
  }
};

struct ArrayExpression : PrimaryExpression {
  ArrayLiteral* array;
  ArrayExpression(ArrayLiteral* array) : array(array)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "ArrayExpression";
  }
};

struct ObjectExpression : PrimaryExpression {
  ObjectLiteral* object;
  ObjectExpression(ObjectLiteral* object) : object(object)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "ObjectExpression";
  }
};

struct PropertyName : Expression {
  Identifier*     identifier = nullptr;
  StringLiteral*  str        = nullptr;
  NumericLiteral* num        = nullptr;
  PropertyName(Identifier* identifier) : identifier(identifier)
  {
  }
  PropertyName(StringLiteral* str) : str(str)
  {
  }
  PropertyName(NumericLiteral* num) : num(num)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "PropertyName";
  }
};

struct PropertyAssignment : Expression {
  enum class Kind { INIT, GET, SET } kind;
  PropertyName* name;
  Expression*   expression = nullptr;
  FunctionBody* body       = nullptr;
  Identifier*   parameter  = nullptr;
  PropertyAssignment(PropertyName* name, Expression* expression)
      : kind(Kind::INIT), name(name), expression(expression)
  {
    if (!name)
      std::cout << "got empty name expression\n";
  }
  PropertyAssignment(PropertyName* name, FunctionBody* body)
      : kind(Kind::GET), name(name), body(body)
  {
  }
  PropertyAssignment(
      PropertyName* name, Identifier* parameter, FunctionBody* body)
      : kind(Kind::SET), name(name), body(body), parameter(parameter)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "PropertyAssignment";
  }
};

struct Elision : Node {
  std::size_t count = 1;
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "Elision";
  }
};

struct ElementList : List<Node> {
  template <typename... Args>
  ElementList(Args&&... args) : List<Node>(std::forward<Args>(args)...)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "ElementList";
  }
};

struct PropertyNameAndValueList : List<PropertyAssignment> {
  template <typename... Args>
  PropertyNameAndValueList(Args&&... args)
      : List<PropertyAssignment>(std::forward<Args>(args)...)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "PropertyNameAndValueList";
  }
};

struct ObjectLiteral : PrimaryExpression {
  PropertyNameAndValueList* declarations;
  ObjectLiteral(PropertyNameAndValueList* declarations = nullptr)
      : declarations(declarations)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "ObjectLiteral";
  }
};

struct MemberExpression : Expression {
  Expression* object;
  Identifier* property;
  Expression* expression;
  MemberExpression(Expression* object, Identifier* property)
      : object(object), property(property), expression(nullptr)
  {
  }
  MemberExpression(Expression* object, Expression* expression)
      : object(object), property(nullptr), expression(expression)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "MemberExpression";
  }
};

struct ArgumentList : List<Expression> {
  template <typename... Args>
  ArgumentList(Args&&... args) : List(std::forward<Args>(args)...)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "ArgumentList";
  }
};

struct Arguments : Expression {
  ArgumentList* list;
  Arguments(ArgumentList* list = nullptr) : list(list)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "Arguments";
  }
};

struct NewExpression : Expression {
  Expression* callee;
  Arguments*  arguments;
  NewExpression(Expression* callee, Arguments* arguments = nullptr)
      : callee(callee), arguments(arguments)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "NewExpression";
  }
};

struct CallExpression : Expression {
  Expression* callee;
  Arguments*  arguments;
  CallExpression(Expression* callee, Arguments* arguments = nullptr)
      : callee(callee), arguments(arguments)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "CallExpression";
  }
};

struct PostfixExpression : Expression {
  std::string op;
  Expression* lhs;
  PostfixExpression(std::string op, Expression* lhs = nullptr)
      : op(op), lhs(lhs)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "PostfixExpression";
  }
};

struct UnaryExpression : Expression {
  std::string op;
  Expression* rhs;
  UnaryExpression(std::string op, Expression* rhs = nullptr) : op(op), rhs(rhs)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "UnaryExpression";
  }
};

struct BinaryExpression : Expression {
  std::string op;
  Expression* lhs;
  Expression* rhs;
  BinaryExpression(
      std::string op, Expression* lhs = nullptr, Expression* rhs = nullptr)
      : op(op), lhs(lhs), rhs(rhs)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "BinaryExpression";
  }
};

struct ConditionalExpression : Expression {
  Expression* test;
  Expression* consequent;
  Expression* alternate;
  ConditionalExpression(
      Expression* test, Expression* consequent, Expression* alternate)
      : test(test), consequent(consequent), alternate(alternate)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "ConditionalExpression";
  }
};

struct AssignmentExpression : BinaryExpression {
  AssignmentExpression(
      std::string op, Expression* lhs = nullptr, Expression* rhs = nullptr)
      : BinaryExpression(op, lhs, rhs)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "AssignmentExpression";
  }
};

struct VariableDeclaration : Expression {
  Identifier* identifier;
  Expression* initializer;
  VariableDeclaration(Identifier* identifier, Expression* initializer = nullptr)
      : identifier(identifier), initializer(initializer)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "VariableDeclaration";
  }
};

struct VariableDeclarationList : List<VariableDeclaration> {
  template <typename... Args>
  VariableDeclarationList(Args&&... args) : List(std::forward<Args>(args)...)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "VariableDeclarationList";
  }
};

struct SourceElement : Node {
};

struct Statement : SourceElement {
};

struct StatementList : List<Statement> {
  template <typename... Args>
  StatementList(Args&&... args) : List(std::forward<Args>(args)...)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "StatementList";
  }
};

struct Block : Statement {
  StatementList* body;
  Block(StatementList* body = nullptr) : body(body)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "Block";
  }
};

struct VariableStatement : Statement {
  VariableDeclarationList* declarations;
  VariableStatement(VariableDeclarationList* declarations)
      : declarations(declarations)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "VariableStatement";
  }
};

struct EmptyStatement : Statement {
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "EmptyStatement";
  }
};

struct ExpressionStatement : Statement {
  Expression* expression;
  ExpressionStatement(Expression* expression) : expression(expression)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "ExpressionStatement";
  }
};

struct IfStatement : Statement {
  Expression* test;
  Statement*  consequent;
  Statement*  alternate;
  IfStatement(
      Expression* test, Statement* consequent, Statement* alternate = nullptr)
      : test(test), consequent(consequent), alternate(alternate)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "IfStatement";
  }
};

struct DoWhileStatement : Statement {
  Statement*  body;
  Expression* test;
  DoWhileStatement(Statement* body, Expression* test) : body(body), test(test)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "DoWhileStatement";
  }
};

struct WhileStatement : Statement {
  Expression* test;
  Statement*  body;
  WhileStatement(Expression* test, Statement* body) : test(test), body(body)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "WhileStatement";
  }
};

struct ForStatement : Statement {
  Node*       init;
  Expression* test;
  Expression* update;
  Statement*  body;
  ForStatement(
      VariableDeclarationList* init, Expression* test = nullptr,
      Expression* update = nullptr, Statement* body = nullptr)
      : init(init), test(test), update(update), body(body)
  {
  }
  ForStatement(
      Expression* init = nullptr, Expression* test = nullptr,
      Expression* update = nullptr, Statement* body = nullptr)
      : init(init), test(test), update(update), body(body)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "ForStatement";
  }
};

struct ForInStatement : Statement {
  VariableDeclaration* decl;
  Expression*          left;
  Expression*          right;
  Statement*           body;
  ForInStatement(
      LeftHandSideExpression* left, Expression* right = nullptr,
      Statement* body = nullptr)
      : decl(nullptr), left(left), right(right), body(body)
  {
  }
  ForInStatement(
      VariableDeclaration* decl, Expression* right = nullptr,
      Statement* body = nullptr)
      : decl(decl), left(nullptr), right(right), body(body)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "ForInStatement";
  }
};

struct ContinueStatement : Statement {
  Identifier* label;
  ContinueStatement(Identifier* label = nullptr) : label(label)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "ContinueStatement";
  }
};

struct BreakStatement : Statement {
  Identifier* label;
  BreakStatement(Identifier* label = nullptr) : label(label)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "BreakStatement";
  }
};

struct ReturnStatement : Statement {
  Expression* argument;
  ReturnStatement(Expression* argument = nullptr) : argument(argument)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "ReturnStatement";
  }
};

struct WithStatement : Statement {
  Expression* object;
  Statement*  body;
  WithStatement(Expression* object, Statement* body)
      : object(object), body(body)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "WithStatement";
  }
};

struct LabelledStatement : Statement {
  Identifier* label;
  Statement*  body;
  LabelledStatement(Identifier* label, Statement* body)
      : label(label), body(body)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "LabelledStatement";
  }
};

struct CaseClause : Node {
  Expression*    test = nullptr;
  StatementList* consequent;
  CaseClause(Expression* test, StatementList* consequent)
      : test(test), consequent(consequent)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "CaseClause";
  }
  bool is_default() const
  {
    return test == nullptr;
  }
};

struct DefaultClause : CaseClause {
  DefaultClause(StatementList* consequent) : CaseClause(nullptr, consequent)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "DefaultClause";
  }
};

struct CaseBlock : List<CaseClause> {
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "CaseBlock";
  }
};

struct SwitchStatement : Statement {
  Expression* discriminant;
  CaseBlock*  cases;
  SwitchStatement(Expression* discriminant, CaseBlock* cases)
      : discriminant(discriminant), cases(cases)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "SwitchStatement";
  }
};

struct ThrowStatement : Statement {
  Expression* argument;
  ThrowStatement(Expression* argument) : argument(argument)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "ThrowStatement";
  }
};

struct TryStatement : Statement {
  Block*      block;
  Identifier* binding;
  Block*      handler;
  Block*      finalizer;
  TryStatement(
      Block* block, Identifier* binding = nullptr, Block* handler = nullptr,
      Block* finalizer = nullptr)
      : block(block), binding(binding), handler(handler), finalizer(finalizer)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "TryStatement";
  }
};

struct DebuggerStatement : Statement {
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "DebuggerStatement";
  }
};

struct Declaration : SourceElement {
};

struct SourceElements : List<SourceElement> {
  template <typename... Args>
  SourceElements(Args&&... args) : List(std::forward<Args>(args)...)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "SourceElements";
  }
};

struct FormalParameterList : List<Identifier> {
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "FormalParameterList";
  }
};

struct FunctionBody : SourceElements {
  FunctionBody(SourceElements* body = nullptr)
      : SourceElements(std::move(body->data))
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "FunctionBody";
  }
};

struct FunctionDeclaration : SourceElement {
  Identifier*          id;
  FormalParameterList* params;
  FunctionBody*        body;
  FunctionDeclaration(
      Identifier* id, FormalParameterList* params, FunctionBody* body)
      : id(id), params(params), body(body)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "FunctionDeclaration";
  }
};

struct FunctionExpression : Expression {
  Identifier*          id;
  FormalParameterList* params;
  FunctionBody*        body;
  FunctionExpression(
      Identifier* id, FormalParameterList* params, FunctionBody* body)
      : id(id), params(params), body(body)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "FunctionExpression";
  }
};

struct ProgramDeclaration : Declaration {
  std::vector<StringLiteral*> directives;
  SourceElements*             body;
  ProgramDeclaration(SourceElements* body = nullptr) : body(body)
  {
  }
  void accept(Visitor& visitor) const override
  {
    return visitor(*this);
  }
  const char* type() const override
  {
    return "ProgramDeclaration";
  }
};

struct Program {

  std::shared_ptr<void> storage;

  ProgramDeclaration* body;

  void accept(Visitor& visitor) const
  {
    return visitor(*this);
  }
};

void accept(const Program& program, Visitor&& visitor);

std::string to_string(const Program& program);

#endif