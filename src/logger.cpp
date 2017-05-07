#include "logger.h"

class SilentLogger final : public Logger {
public:
  void log(const std::string &what) final {}
};

Logger* make_silent_logger()
{
  static SilentLogger instance;
  return &instance;
}

Logger &operator<<(Logger &out, const std::string &str)
{
  out.log(str);
  return out;
}
