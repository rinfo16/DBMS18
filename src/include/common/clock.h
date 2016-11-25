#ifndef CLOCK_H_
#define CLOCK_H_

#include "common/define.h"
#include <string>
#include <time.h>

namespace utils {

class Clock {
 public:

  Clock();

  ~Clock();

  void Start();
  void Stop();

  uint64_t Elapse() const ;

  std::string Format() const ;
 private:
  uint64_t duration;
  clock_t start, stop;
};

}  // namespace utils

#endif // CLOCK_H_
