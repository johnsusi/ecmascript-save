#include "reference.h"

struct Reference::Private {
  Type    base;
  String  name;
  Boolean strict;
};

Type GetBase(const Reference& V)
{
  return V.pimpl->base;
}

String GetReferencedName(const Reference& V)
{
  return V.pimpl->name;
}

Boolean IsStrictReference(const Reference& V)
{
  return V.pimpl->strict;
}

Boolean HasPrimitiveBase(const Reference& V)
{
  auto base = GetBase(V);
  return base.IsBoolean() || base.IsString() || base.IsNumber();
}

Boolean IsPropertyReference(const Reference& V)
{
  return HasPrimitiveBase(V) || GetBase(V).IsObject();
}

Boolean IsUnresolvableReference(const Reference& V)
{
  return V.pimpl->base.IsUndefined();
}

// 8.7.1 GetValue (V)
Type GetValue(const Type& V)
{
  // 1. If Type(V) is not Reference, return V.
  if (!V.IsReference()) return V;

  // 2. Let base be the result of calling GetBase(V).
  auto base = GetBase(V);

  // 3. If IsUnresolvableReference(V), throw a ReferenceError exception.
  if (IsUnresolvableReference(V)) return ReferenceError();

  // 4. If IsPropertyReference(V), then
  if (IsPropertyReference(V)) {
    // a. If HasPrimitiveBase(V) is false, then let get be the [[Get]] internal
    // method of base, otherwise let get be the special [[Get]] internal method
    // defined below.
    if (!HasPrimitiveBase(base)) {
      // b. Return the result of calling the get internal method using base as
      // its
      // this value, and passing GetReferencedName(V) for the argument.

      return ToObject(V).Get(GetReferencedName(V));
    }
    else {

      // 1. Let O be ToObject(base).
      auto O = ToObject(base);

      // 2. Let desc be the result of calling the [[GetProperty]] internal
      // method of O with property name P.
      // auto desc = O.GetProperty(P);
      // 3. If desc is undefined, return undefined.
      // 4. If IsDataDescriptor(desc) is true, return desc.[[Value]].
      // if (IsDataDescriptor(desc)) return desc.Value();

      // 5. Otherwise, IsAccessorDescriptor(desc) must be true so, let getter be
      // desc.[[Get]] (see 8.10).

      // 6. If getter is undefined, return undefined.
      // 7. Return the result calling the [[Call]] internal method of getter
      // providing base as the this value and providing no arguments.
    }
  }
  // 5. Else, base must be an environment record.
  // a. Return the result of calling the GetBindingValue (see 10.2.1) concrete
  // method of base passing GetReferencedName(V) and IsStrictReference(V) as
  // arguments.

  return GetBindingValue(V, GetReferenceName(V), IsStrictReference(V));
}