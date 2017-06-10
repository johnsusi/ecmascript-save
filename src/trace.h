#ifndef ECMASCRIPT_TRACE_H
#define ECMASCRIPT_TRACE_H

#include "config.h"

#include <string>
#include <vector>

#include <boost/current_function.hpp>

std::vector<std::string>& stack_trace();

struct TraceGuard {
  const char* func;
  const char* file;
  int         line;

  TraceGuard(const char* func, const char* file, int line);
  ~TraceGuard() noexcept;
};

#define trace(...)                                                             \
  TraceGuard tg_##__LINE__(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__)

#endif