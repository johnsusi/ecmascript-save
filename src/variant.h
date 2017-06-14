#ifndef ECMASCRIPT_VARIANT_H
#define ECMASCRIPT_VARIANT_H

#include "config.h"

#if defined(HAVE_STD_VARIANT)

#include <variant>

template <typename... Ts>
using variant = std::variant<Ts...>;

#elif defined(HAVE_STD_EXPERIMENTAL_VARIANT)

#include <experimental/variant>

template <typename... Ts>
using variant = std::experimental::variant<Ts...>;

#elif defined(HAVE_MPARK_VARIANT)

#include <mpark/variant.hpp>

template <typename... Ts>
using variant = mpark::variant<Ts...>;

#elif defined(HAVE_BOOST_VARIANT)

#include <boost/variant.hpp>

template <typename... Ts>
using variant = boost::variant<Ts...>;

#endif

#endif