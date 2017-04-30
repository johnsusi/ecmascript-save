#include "parser.h"

bool is_assignment_operator(const std::string &op)
{
  return op == "=" || op == "*=" || op == "/=" || op == "%=" || op == "+=" ||
         op == "-=" || op == "<<=" || op == ">>=" || op == ">>>=" ||
         op == "&=" || op == "^=" || op == "|=";
}


