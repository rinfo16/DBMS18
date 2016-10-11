#include <iostream>
#include <string>
#include "common/hash.h"
int main(int argc, const char* argv[]) {
  std::cout << "Hash unit test !!!" << std::endl;
  std::string s = "Hash unit test !!!";
  int32_t i32 = utils::Hash32((void*)s.c_str(), s.size());
  int64_t i64 = utils::Hash64((void*)s.c_str(), s.size());
  return 0;
}
