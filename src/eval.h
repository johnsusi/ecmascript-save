#ifndef ECMASCRIPT_EVAL_H
#define ECMASCRIPT_EVAL_H

#include "ast.h"

ast::Program eval(const std::string &);
ast::Program eval(const std::u16string &);

#endif