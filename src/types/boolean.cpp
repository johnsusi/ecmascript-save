#include "boolean.h"

Boolean::Boolean() : value(false)
{
}

Boolean::Boolean(bool value) : value(value)
{
}

Boolean::operator bool() const
{
  return value;
}

bool Boolean::operator==(const Boolean& other) const
{
  return value == other.value;
}

bool Boolean::operator!=(const Boolean& other) const
{
  return !(operator==(other));
}
