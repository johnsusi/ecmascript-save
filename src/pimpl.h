#ifndef ECMASCRIPT_PIMPL_H
#define ECMASCRIPT_PIMPL_H

#include <memory>

template <typename T>
using Pimpl = const std::unique_ptr<T>;

// Rules for implementing the Pimpl Idiom

#endif