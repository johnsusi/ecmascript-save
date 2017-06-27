#ifndef ECMASCRIPT_TRACE_H
#define ECMASCRIPT_TRACE_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <vector>

// #include <boost/current_function.hpp>

std::vector<std::string>& stack_trace();

struct TraceGuard {
  const char* func;
  const char* file;
  int         line;

  TraceGuard(const char* func, const char* file, int line);
  ~TraceGuard() noexcept;
};

#define trace(...)                                                             \
  TraceGuard tg_##__LINE__(__PRETTY_FUNCTION__, __FILE__, __LINE__)

#endif