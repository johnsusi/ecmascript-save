#ifndef ECMASCRIPT_OPTIONAL_H
#define ECMASCRIPT_OPTIONAL_H

#include "config.h"

#if defined(HAVE_STD_OPTIONAL)

#include <optional>
template <typename T>
using optional = std::optional<T>;

#elif defined(HAVE_STD_EXPERIMENTAL_OPTIONAL)

#include <experimental/optional>
template <typename T>
using optional = std::experimental::optional<T>;

#elif defined(HAVE_BOOST_OPTIONAL)

#include <boost/optional.hpp>
template <typename T>
using optional = boost::optional<T>;

#endif

#endif