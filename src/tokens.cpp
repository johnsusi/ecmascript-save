#include "tokens.h"

struct Tokens::Private {

  mutable std::vector<Token>                           tokens;
  // std::vector<std::unique_ptr<DebugInfo>>      debug_infos;
  std::vector<std::unique_ptr<std::u16string>> strings;

  Private()
  {
  }
  Private(std::vector<Token> tokens) : tokens(tokens)
  {
  }
};

Tokens::Tokens() : pimpl(std::make_shared<Private>())
{
}

Tokens::Tokens(std::vector<Token> tokens)
    : pimpl(std::make_shared<Private>(std::move(tokens)))
{
}

Tokens::~Tokens() = default;

void Tokens::add(Token token) {
  pimpl->tokens.push_back(std::move(token));
}

std::vector<Token>::const_iterator Tokens::begin() const
{
  return pimpl->tokens.begin();
}

std::vector<Token>::const_iterator Tokens::end() const
{
  return pimpl->tokens.end();
}

Tokens::operator bool() const
{
  return !!pimpl;
}
