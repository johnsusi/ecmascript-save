#ifndef ECMASCRIPT_SYNTAX_ERROR_H
#define ECMASCRIPT_SYNTAX_ERROR_H

#include <stdexcept>

class SyntaxError : public std::runtime_error
{

  static std::string full_error(const std::string& f, const std::string& s, int x, int y)
  {
    return f + ":" + std::to_string(y) + ": SyntaxError: " + s;
  }

public:
  SyntaxError(const std::string& filename, const std::string& what_arg,
    int col = -1, int row = -1)
    : std::runtime_error( full_error(filename, what_arg, col, row) ) {}
};


#endif