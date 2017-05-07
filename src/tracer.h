#ifndef ECMASCRIPT_TRACER_H
#define ECMASCRIPT_TRACER_H

#include <vector>

class Tracer {
  struct Line {
    const char* method;
    const char* file;
    int         line;
    Line(const char* method, const char* file, int line)
        : method(method), file(file), line(line)
    {
    }
  };
  std::vector<Line> m_stack;

public:
  void                     enter(const char*, const char*, int);
  void                     leave();
  const std::vector<Line>& stack() const;
  auto                     begin() const { return m_stack.rbegin(); }
  auto                     end() const { return m_stack.rend(); }
};

Tracer& tracer();

class TraceGuard {
public:
  TraceGuard(const char*, const char* = "unnamed", int = 1);
  ~TraceGuard();
};

#endif