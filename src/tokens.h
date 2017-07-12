#ifndef ECMASCRIPT_TOKENS_H
#define ECMASCRIPT_TOKENS_H

#include "token.h"

#include <memory>
#include <vector>

class Tokens {
  struct Private;
  std::shared_ptr<const Private> pimpl;

public:
  Tokens();
  Tokens(std::vector<Token> tokens);
  ~Tokens();

  void add(Token token);

  std::vector<Token>::const_iterator begin() const;
  std::vector<Token>::const_iterator end() const;
  explicit operator bool() const;
};

#endif