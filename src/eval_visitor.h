#ifndef ECMASCRIPT_EVAL_VISITOR_H
#define ECMASCRIPT_EVAL_VISITOR_H

#include "basic_visitor.h"

class EvalVisitor : public BasicVisitor {
public:
  std::string str() const { return ""; }
};

#endif