#ifndef ECMASCRIPT_TYPES_REFERENCE_H
#define ECMASCRIPT_TYPES_REFERENCE_H

#include "boolean.h"
#include "number.h"
#include "string.h"
#include "undefined.h"

struct Type;

struct Reference {
  struct Private;
  const std::unique_ptr<Private> pimpl;
};

Type    GetBase(const Reference&);
String  GetReferencedName(const Reference&);
Boolean IsStrictReference(const Reference&);
Boolean HasPrimitiveBase(const Reference&);
Boolean IsPropertyReference(const Reference&);
Boolean IsUnresolvableReference(const Reference&);
Type GetValue(Reference V);

#endif