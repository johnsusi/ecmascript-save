#ifndef ECMASCRIPT_EVAL_H
#define ECMASCRIPT_EVAL_H

#include "options.h"
#include "source.h"
#include "visitor.h"

void eval(const Source&, Visitor&, const Options&);

#endif