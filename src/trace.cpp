#include "trace.h"

#include <exception>

using namespace std::literals::string_literals;

std::vector<std::string>& stack_trace()
{
  static std::vector<std::string> instance;
  return instance;
}

TraceGuard::TraceGuard(const char* func, const char* file, int line)
    : func(func), file(file), line(line)
{
}

TraceGuard::~TraceGuard() noexcept
{
  try {
    if (std::uncaught_exception()) {
      auto f = std::string(file);
      auto n = f.find_last_of('/');

      if (n != std::string::npos) f = f.substr(n + 1);

      stack_trace().emplace_back(
          "    at "s + func + " (" + f + ":" + std::to_string(line) + ")");
    }
  }
  catch (...) {
  }
}
