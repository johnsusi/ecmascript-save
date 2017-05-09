#include "tracer.h"

#include <sstream>
#include <string>

void Tracer::enter(std::string method, std::string file, int line)
{
  m_stack.emplace_back(method, file, line);
}
void Tracer::leave()
{
  m_stack.pop_back();
}

const std::vector<Tracer::Line>& Tracer::stack() const
{
  return m_stack;
}

Tracer& tracer()
{
  static Tracer instance;
  return instance;
}

TraceGuard::TraceGuard(std::string method, std::string file, int line)
{
  tracer().enter(method, file, line);
}

TraceGuard::~TraceGuard()
{
  tracer().leave();
}

std::string stack_trace()
{
  std::stringstream buf;
  for (auto row : tracer()) {
    buf << "    at " << row.method << " (" << row.file << ":" << row.line
        << ")\n";
  }
  return buf.str();
}
