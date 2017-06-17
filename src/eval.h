#ifndef ECMASCRIPT_EVAL_H
#define ECMASCRIPT_EVAL_H

// #include "ecmascript_export.h"
#include "visitor.h"

#include <string>

void eval(const std::string&, Visitor&, bool = false, bool = true, bool = true);
void eval(
    const std::u16string&, Visitor&, bool verbose = false, bool parse = true,
    bool run = true);

#endif