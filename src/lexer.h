#ifndef ECMASCRIPT_LEXER_H
#define ECMASCRIPT_LEXER_H

#include "optional.h"
#include "source.h"
#include "token.h"

#include <iterator>
#include <memory>
#include <string>
#include <vector>

class Lexer {

  struct Private;
  const std::shared_ptr<Private> pimpl; // flyweight

  const std::vector<Token>& tokens() const;

public:
  Lexer(std::vector<Token>);
  Lexer(Source);

  Lexer(const char* source) : Lexer(Source::from_utf8(source))
  {
  }
  Lexer(const std::string& source) : Lexer(Source::from_utf8(source))
  {
  }

  auto begin() const
  {
    return tokens().begin();
  }

  auto end() const
  {
    return tokens().end();
  }

  struct DebugInfo {
    virtual ~DebugInfo()
    {
    }
    virtual std::string syntax_error_at() const = 0;
    virtual std::string loc() const             = 0;
  };

  DebugInfo* lookup(const std::vector<Token>::const_iterator&) const;
};

bool operator==(const Lexer& lhs, const Lexer& rhs);

bool operator!=(const Lexer& lhs, const Lexer& rhs);

std::ostream& operator<<(std::ostream& out, const Lexer& lexer);

#endif