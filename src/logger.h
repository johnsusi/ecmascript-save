#ifndef ECMASCRIPT_LOGGER_H
#define ECMASCRIPT_LOGGER_H

#include <functional>
#include <memory>
#include <sstream>

class Logger {

public:
  virtual ~Logger()
  {
  }
  virtual void log(const std::string&) = 0;

  template <typename T>
  auto log(T&& callback) -> decltype(callback(*this), void())
  {
    callback(*this);
  }
};

Logger& operator<<(Logger& out, const std::string& str);

template <typename T>
Logger& operator<<(Logger& out, const T& value)
{
  std::stringstream buf;
  buf << value;
  out.log(buf.str());
  return out;
}
template <typename T>
class StandardLogger : public Logger {
  T& out;

public:
  StandardLogger(T& out) : out(out)
  {
  }
  void log(const std::string& what) override
  {
    out << what;
  }
};

template <typename T>
std::unique_ptr<Logger> make_standard_logger(T& arg)
{
  return std::make_unique<StandardLogger<T>>(arg);
}

Logger* make_silent_logger();

#endif