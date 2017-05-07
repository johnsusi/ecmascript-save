#include "tracer.h"

void Tracer::enter(const char* method, const char* file, int line)
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

TraceGuard::TraceGuard(const char* method, const char* file, int line)
{
  tracer().enter(method, file, line);
}

TraceGuard::~TraceGuard()
{
  tracer().leave();
}
