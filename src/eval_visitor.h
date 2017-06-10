#ifndef ECMASCRIPT_EVAL_VISITOR_H
#define ECMASCRIPT_EVAL_VISITOR_H

#include "basic_visitor.h"
#include "runtime.h"

// struct LexicalEnvironment;
// struct EnvironmentRecord;
// struct ExecutionContext {
//   LexicalEnvironment* LexicalEnvironment();
//   LexicalEnvironment* VariableEnvironment();
//   Type                ThisBinding();
// };

class EvalVisitor : public BasicVisitor {
  //   Type                                           GlobalObject;
  //   std::unique_ptr<LexicalEnvironment>            GlobalEnvironment;
  //   std::vector<std::unique_ptr<ExecutionContext>> stack;

public:
  std::string str() const { return "EvalVisitor"; }
  //   void operator(const Program& program) const
  //   {
  //     stack.push_back(new ExecutionContext(
  //         GlobalEnironment.get(), GlobalEnvironment.get(), GlobalObject));

  //   }
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