#include "common/datetime.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

// ISO 8601 extended format (CCYY-MM-DD)
// EX. 2016-10-25
using namespace boost::gregorian;
using namespace boost::posix_time;

int64_t DateSimpleString2Subsec(const std::string &date_simple_string) {
  date d(from_simple_string(date_simple_string));
  date epoch(1970, 1, 1);
  time_duration duration = ptime(d) - ptime(epoch);
  return duration.total_microseconds();
}

std::string DateSubsec2SimpleString(int64_t sub_sec) {
  date epoch(1970, 1, 1);
  date d = (ptime(epoch) + microseconds(sub_sec)).date();
  return boost::gregorian::to_simple_string(d);
}

