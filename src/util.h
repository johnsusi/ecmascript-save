#ifndef ECMASCRIPT_UTIL_H
#define ECMASCRIPT_UTIL_H

#include <string>

#include <boost/core/demangle.hpp>

std::string left_align_text(
    const std::string &text, const std::string &indent = "");

const char *u_charCategory(int c);

std::string read_file(const std::string &filename);

std::string read_stdin();

template <typename T>
std::string demangle(T)
{
  return boost::core::demangle(typeid(T).name());
}

template <class F, size_t... Is>
constexpr auto index_apply_impl(F f, std::index_sequence<Is...>)
{
  return f(std::integral_constant<size_t, Is>{}...);
}

template <size_t N, class F>
constexpr auto index_apply(F f)
{
  return index_apply_impl(f, std::make_index_sequence<N>{});
}

#endif