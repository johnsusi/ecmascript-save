#ifndef ECMASCRIPT_OBJECT_H
#define ECMASCRIPT_OBJECT_H

#include <memory>

#include "boolean.h"
#include "property_descriptor.h"
#include "string.h"

class Type;

class Object {

  using Callable = Type (*)(std::size_t, Type**);

  struct Private;
  std::unique_ptr<Private> pimpl;

public:
  Object();
  Object(const Object&);
  Object& operator=(const Object&);
  ~Object();
  Callable operator[](const char*) const;

  Boolean Extensible();

  Type   Prototype();
  String Class();
  Type   Get(const String&);

  // GetOwnProperty(const String&)

  // GetProperty(const String&);
  void    Put(const String&, const Type&, const Boolean&);
  Boolean CanPut(const String&);
  Boolean HasProperty(const String&);
  Boolean Delete(const String&, const Boolean&);
  Type    DefaultValue(const String&);
  Boolean
  DefineOwnProperty(const String&, const PropertyDescriptor&, const Boolean&);
};

#endif