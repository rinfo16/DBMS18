#ifndef CLOCK_H_
#define CLOCK_H_

#include "common/define.h"
#include <string>

namespace utils {

class Clock {
 public:

  Clock();

  ~Clock();

  void Start() {

  }
  void Stop() {

  }

  uint64_t Elapse() const {
    return 0;
  }

  std::string Format(std::string & format) const {
    return "";
  }

};

}  // namespace utils

#endif // CLOCK_H_
