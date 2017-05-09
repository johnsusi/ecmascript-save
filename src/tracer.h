#ifndef ECMASCRIPT_TRACER_H
#define ECMASCRIPT_TRACER_H

#include <string>
#include <vector>

class Tracer {
  struct Line {
    std::string method;
    std::string file;
    int         line;
    Line(std::string method, std::string file, int line)
        : method(method), file(file), line(line)
    {
    }
  };
  std::vector<Line> m_stack;

public:
  void                     enter(std::string, std::string, int);
  void                     leave();
  const std::vector<Line>& stack() const;
  auto                     begin() const { return m_stack.rbegin(); }
  auto                     end() const { return m_stack.rend(); }
};

Tracer& tracer();

class TraceGuard {
public:
  TraceGuard(std::string, std::string = "unnamed", int = 1);
  ~TraceGuard();
};

std::string stack_trace();

#endif