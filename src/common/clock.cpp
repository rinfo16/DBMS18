#include "common/clock.h"
#include <time.h>
#include <iostream>
namespace utils {

Clock::Clock() {
  // TODO Auto-generated constructor stub
duration = 0;
start = 0;
stop = 0;
}

Clock::~Clock() {
  // TODO Auto-generated destructor stub
}

void Clock::Start(){
  start = clock();
}

void Clock::Stop(){
  stop = clock();
  duration = stop-start;
}
uint64_t Clock::Elapse() const {
  return duration;
}
std::string Clock::Format() const {
  std::cout << "elapsed time = ";
  std::cout << duration;
  std::cout << "ms" << std::endl;
//  printf("elapsed time = %ld ms",duration);
}
}  // namespace utils
