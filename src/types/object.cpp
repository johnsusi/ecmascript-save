#include "object.h"

#include <string>
#include <unordered_map>

struct Object::Private {

  std::unordered_map<std::string, Callable> internals;

  Object* prototype  = nullptr;
  bool    extensible = true;
};

Object::Object() : pimpl(new Private())
{
}

Object::Object(const Object& other) : pimpl(new Private()) // TODO
{
}

Object& Object::operator=(const Object& other)
{
  // TODO
  return *this;
}

Object::~Object()
{
}

Object::Callable Object::operator[](const char* method) const
{
  return pimpl->internals.at(method);
}

// struct Property {

//   Type value        = Undefined{};
//   Type get          = Undefined{};
//   Type set          = Undefined{};
//   bool writable     = false;
//   bool enumerable   = false;
//   bool configurable = false;
// };