#ifndef ECMASCRIPT_LEXER_H
#define ECMASCRIPT_LEXER_H

#include "optional.h"
#include "source.h"
#include "token.h"

#include <memory>
#include <string>
#include <vector>

class Lexer {

  struct LexicalGrammar;
  const std::shared_ptr<LexicalGrammar> grammar;

public:
  using iterator = std::vector<Token>::const_iterator;

  Lexer();
  Lexer(std::initializer_list<Token>);
  Lexer(std::vector<Token>);
  Lexer(Source);
  Lexer(const char*);
  Lexer(const char16_t*);

  // template <typename... Args>
  // Lexer(Args&&... args) :
  // Lexer(std::vector<Token>{std::forward<Args>(args)...})
  // {
  // }

  iterator begin() const;
  iterator end() const;

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