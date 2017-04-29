#ifndef ECMASCRIPT_EVAL_H
#define ECMASCRIPT_EVAL_H

#include "ast.h"

Program eval(const std::string &);
Program eval(const std::u16string &);

#endif