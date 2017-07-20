#include "token.h"

std::ostream& operator<<(std::ostream& out, const Token& token)
{
  return out << token.to_string();
}

std::u16string& create_static_string(std::u16string value)
{
  static std::vector<std::unique_ptr<std::u16string>> storage;
  storage.emplace_back(std::make_unique<std::u16string>(std::move(value)));
  return *storage.back();
}
