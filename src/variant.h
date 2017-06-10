#ifndef ECMASCRIPT_VARIANT_H
#define ECMASCRIPT_VARIANT_H

#include "config.h"

#if defined(HAVE_STD_VARIANT)

#include <variant>

template <typename T>
using variant = std::variant<T>;

#elif defined(HAVE_STD_EXPERIMENTAL_VARIANT)

#include <experimental/variant>

template <typename T>
using variant = std::experimental::variant<T>;

#elif defined(HAVE_MPARK_VARIANT)

#include <mpark/variant.hpp>

template <typename T>
using variant = mpark::variant<T>;

#elif defined(HAVE_BOOST_VARIANT)

#include <boost/variant.hpp>

template <typename T>
using variant = boost::variant<T>;

#endif

#endif