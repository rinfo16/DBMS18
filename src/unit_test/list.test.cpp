#include "common/list.hpp"
#include <iostream>

class C {
};

int main(int argc, const char* argv[]) {
  std::cout << "List unit test !!!" << std::endl;
  utils::List<C> list;
  utils::List<C> *first = list.First();
}
