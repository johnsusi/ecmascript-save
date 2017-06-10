#ifndef ECMASCRIPT_EVAL_VISITOR_H
#define ECMASCRIPT_EVAL_VISITOR_H

#include "basic_visitor.h"
#include "runtime.h"

#include <memory>
#include <vector>

// struct LexicalEnvironment;
// struct EnvironmentRecord;
// struct ExecutionContext {
//   LexicalEnvironment* LexicalEnvironment();
//   LexicalEnvironment* VariableEnvironment();
//   Type                ThisBinding();
// };

struct LexicalEnvironment;

struct ExecutionContext {
  LexicalEnvironment* LexicalEnvironemnt;
  LexicalEnvironment* VariableEnvironment;
  Type                ThisBinding;
};

class EvalVisitor : public BasicVisitor {
  //   Type                                           GlobalObject;
  //   std::unique_ptr<LexicalEnvironment>            GlobalEnvironment;
  std::vector<std::unique_ptr<ExecutionContext>> stack;
  ExecutionContext*                              context;
  void InitializeGlobalExecutionContext() {}

public:
  std::string str() const { return "EvalVisitor"; }

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