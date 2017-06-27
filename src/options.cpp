#include "options.h"

#include <iostream>

auto Options::ReadOrWrite::data() -> decltype(dataRef.lock())
{
  if (auto data = dataRef.lock())
    return data;
  else
    throw std::runtime_error("Trying to access invalid Data");
}

auto Options::ReadOrWrite::data() const -> decltype(dataRef.lock())
{
  if (auto data = dataRef.lock())
    return data;
  else
    throw std::runtime_error("Trying to access invalid Data");
}

Options::ReadOrWrite& Options::ReadOrWrite::operator=(const char* value)
{
  return operator=(std::string{value});
}

const Options::Option& Options::ReadOrWrite::value() const
{
  return data()->at(key);
}

Options::Options()
{
}

Options::Options(std::initializer_list<std::pair<Key, Options::Option>> il)
{
  for (auto& p : il)
    data->insert(p);
}

Options::ReadOrWrite Options::operator[](const Key& key)
{
  return {data, key};
}

const Options::ReadOrWrite Options::operator[](const Key& key) const
{
  return {data, key};
}

std::size_t Options::count(const Key& key) const
{
  return data->count(key);
}

inline std::ostream&
operator<<(std::ostream& out, const std::vector<std::string>& value)
{
  auto it = value.begin();
  out << "[";
  if (it != value.end())
    out << *it++;
  while (it != value.end())
    out << "," << *it++;
  out << "]";
  return out;
}

std::ostream& operator<<(std::ostream& out, const Options::ReadOrWrite& value)
{
  using namespace std;
  visit([&](auto value) { out << value; }, value.value());
  return out;
}
