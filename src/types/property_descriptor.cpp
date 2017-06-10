#include "property_descriptor.h"

// 8.10.1 IsAccessorDescriptor ( Desc )
Boolean IsAccessorDescriptor(const Type& Desc)
// When the abstract operation IsAccessorDescriptor is called with property
// descriptor Desc, the following steps are taken:
{
  // 1. If Desc is undefined, then return false.
  if (Desc == Undefined{}) return false;

  // 2. If both Desc.[[Get]] and Desc.[[Set]] are absent, then return false.
  if () }

3. Return true.
