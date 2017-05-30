#include "trace.h"

#include <sstream>
#include <string>

void Trace::enter(std::string method, std::string file, int line)
{
  m_stack.emplace_back(method, file, line);
}
void Trace::leave()
{
  m_stack.pop_back();
}

const std::vector<Trace::Line>& Trace::stack() const
{
  return m_stack;
}

Trace& Trace::instance()
{
  static Trace instance;
  return instance;
}

TraceGuard::TraceGuard(std::string method, std::string file, int line)
{
  Trace::instance().enter(method, file, line);
}

TraceGuard::~TraceGuard()
{
  Trace::instance().leave();
}

std::string stack_trace()
{
  std::stringstream buf;
  for (auto row : Trace::instance()) {
    buf << "    at " << row.method << " (" << row.file << ":" << row.line
        << ")\n";
  }
  return buf.str();
}
