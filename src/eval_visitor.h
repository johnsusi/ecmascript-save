#ifndef ECMASCRIPT_EVAL_VISITOR_H
#define ECMASCRIPT_EVAL_VISITOR_H

#include "basic_visitor.h"
#include "optional.h"
#include "runtime.h"
#include "variant.h"

#include <memory>
#include <vector>

// struct LexicalEnvironment;
// struct EnvironmentRecord;
// struct ExecutionContext {
//   LexicalEnvironment* LexicalEnvironment();
//   LexicalEnvironment* VariableEnvironment();
//   Type                ThisBinding();
// };

struct Completion {
  enum class Type { NORMAL, BREAK, CONTINUE, RETURN, THROW } type;
  optional<Type>  value;
  optional<Token> target;
};

struct LexicalEnvironment;

struct ExecutionContext {
  LexicalEnvironment* LexicalEnvironemnt;
  LexicalEnvironment* VariableEnvironment;
  Type                ThisBinding;
};

struct EvalExpressionVisitor : public BasicVisitor {
  std::vector<Type> stack;

  void operator()(const BinaryExpression& expr) override
  {
    std::cout << expr.op << "\n";
    apply(expr.lhs);
    apply(expr.rhs);

    auto rhs = stack.back();
    stack.pop_back();
    auto lhs = stack.back();
    stack.pop_back();

    if (expr.op == "+") {
      Number result = ToNumber(lhs) + ToNumber(rhs);
      stack.push_back(result);
    }
  }

  // void operator()(const NumericLiteral& literal) override
  // {
  //   stack.push_back(literal.value);
  // }
};

class EvalVisitor : public BasicVisitor {
  //   Type                                           GlobalObject;
  //   std::unique_ptr<LexicalEnvironment>            GlobalEnvironment;
  std::vector<std::unique_ptr<ExecutionContext>> stack;

  ExecutionContext* context = nullptr;

  void InitializeGlobalExecutionContext()
  {
  }

public:
  std::string str() const
  {
    return "EvalVisitor";
  }

  void operator()(const Program& program) override
  // 10.4.1 Entering Global Code
  {
    // 1. Initialise the execution context using the global code as described in
    // 10.4.1.1.

    // 10.4.1.1 Initial Global Execution Context
    // The following steps are performed to initialise a global execution
    // context for ECMAScript code C:

    stack.emplace_back();
    context = stack.back().get();

    // 1. Set the VariableEnvironment to the Global Environment.
    // 2. Set the LexicalEnvironment to the Global Environment.
    // 3. Set the ThisBinding to the global object.

    // 2. Perform Declaration Binding Instantiation as described in 10.5 using
    // the global code.
    std::cout << "Entering global code\n";
    // stack.push_back(new ExecutionContext(
    //     GlobalEnironment.get(), GlobalEnvironment.get(), GlobalObject));
    apply(program.body);
    // for (auto stmt : *program.body->body) std::cout << "stmt\n";
  }

  void operator()(const ExpressionStatement& stmt) override
  {
    // 1. Let exprRef be the result of evaluating Expression.
    // 2. Return (normal, GetValue(exprRef), empty).

    EvalExpressionVisitor v;
    v.apply(stmt.expression);
    std::cout << ToNumber(v.stack.back()) << std::endl;
  }
};

// DeclarationBindingInstantiation(LexicalEnvironment* env, Boolean
// configurableBindings, Boolean strict)
// {

// 1. Let env be the environment record component of the running execution
// context‘s VariableEnvironment.
// 2. If code is eval code, then let configurableBindings be true else let
// configurableBindings be false.
// 3. If code is strict mode code, then let strict be true else let strict be
// false.

// }

#endif