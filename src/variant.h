#ifndef ECMASCRIPT_VARIANT_H
#define ECMASCRIPT_VARIANT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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

using namespace mpark;

// template <typename... Ts>
// using variant = mpark::variant<Ts...>;

#elif defined(HAVE_BOOST_VARIANT)

#include <type_traits>
#include <utility>

#include <boost/variant.hpp>
#include <boost/variant/get.hpp>

template <typename... Ts>
using variant = boost::variant<Ts...>;

#define get_if boost::get
#define visit boost::apply_visitor

#endif

#endif