#ifndef ECMASCRIPT_TRACE_H
#define ECMASCRIPT_TRACE_H

#include <string>
#include <vector>

class Trace {
  struct Line {
    std::string method;
    std::string file;
    int line;
    Line(std::string method, std::string file, int line)
        : method(method), file(file), line(line) {}
  };
  std::vector<Line> m_stack;

public:
  void enter(std::string, std::string, int);
  void leave();
  const std::vector<Line> &stack() const;
  auto begin() const { return m_stack.rbegin(); }
  auto end() const { return m_stack.rend(); }

  static Trace &instance();
};

class TraceGuard {
public:
  TraceGuard(std::string, std::string = "unnamed", int = 1);
  ~TraceGuard();
};

std::string stack_trace();

#ifdef ECMASCRIPT_RELEASE

#define trace(method)

#else

#define trace(method)                                                          \
  TraceGuard tg_##__LINE__(                                                    \
      boost::typeindex::type_id<decltype(*this)>().pretty_name() +             \
          "::" + __func__,                                                     \
      __FILE__, __LINE__)

#endif

#endif