#include <iostream>
#include "../include/common/list.hpp"

class C {
};

int main(int argc, const char* argv[]) {
  std::cout << "List unit test !!!" << std::endl;
  utils::ListNode<C> list;
  utils::ListNode<C> *first = list.First();
}
