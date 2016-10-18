#include <iostream>
#include <string>
#include <sstream>
#include "../include/common/list.hpp"


int main(int argc, const char* argv[]) {
  std::cout << "List unit test !!!" << std::endl;

  utils::List<std::string> list;
  for (int i = 0; i < 1000; i++)
  {
    utils::ListNode<std::string> * node = new utils::ListNode<std::string>();
    std::stringstream ssm;
    ssm << i;
    node->append(ssm.str());
    list.PushBack(node);
  }
  utils::ListNode<std::string> *node = list.PopFront();
  for (int i = 0; node != NULL; i++)
  {
    std::cout << i << " " << *node << std::endl;
    node = list.PopFront();
  }
  return 0;
}
