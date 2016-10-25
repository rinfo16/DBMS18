#ifndef DATETIME_H_
#define DATETIME_H_

#include <string>
#include "common/define.h"

// ISO 8601 extended format (CCYY-MM-DD)
// EX. 2016-10-25

int64_t DateSimpleString2Subsec(const std::string &date_simple_string);
std::string DateSubsec2SimpleString(int64_t sub_sec);

#endif // DATETIME_H_
