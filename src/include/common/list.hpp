#ifndef SRC_INCLUDE_COMMON_LIST_HPP_
#define SRC_INCLUDE_COMMON_LIST_HPP_

#include "common/define.h"

namespace utils {

template<typename T>
class List : public T {
 public:
  List()
      : prev_(NULL),
        next_(NULL) {
  }

  bool IsFirst() const {
    return false;
  }

  bool IsLast() const {
    return false;
  }
  void LinkToRight(List<T> *right_node) {
  }

  void LinkToLeft(List<T> *left_node) {
  }

  List<T>* First() {
    return NULL;
  }

  List<T>* Last() {
    return NULL;
  }

  void RemoveThis() {

  }

 private:
  List<T> *prev_;
  List<T> *next_;
};

}  // namespace utils

#endif // SRC_INCLUDE_COMMON_LIST_HPP_
