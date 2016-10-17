#ifndef SRC_INCLUDE_COMMON_LIST_HPP_
#define SRC_INCLUDE_COMMON_LIST_HPP_

#include "common/define.h"

namespace utils {

template<typename T>
class ListNode : public T {
 public:
  ListNode()
      : prev_(NULL),
        next_(NULL) {
  }

  bool IsFirst() const {
    return false;
  }

  bool IsLast() const {
    return false;
  }
  void LinkToRight(ListNode<T> *right_node) {
  }

  void LinkToLeft(ListNode<T> *left_node) {
  }

  ListNode<T>* First() {
    return NULL;
  }

  ListNode<T>* Last() {
    return NULL;
  }

  void Remove() {

  }

  ListNode<T>* Next() {
    return NULL;
  }
  ListNode<T>* Prev() {
    return NULL;
  }

 private:
  ListNode<T> *prev_;
  ListNode<T> *next_;
};

template<typename T>
class List {
 public:
  List()
      : head_(NULL),
        tail_(NULL) {
  }
  ;
  void PushBack(ListNode<T> *node) {
  }
  void PushFront(ListNode<T> *node) {
  }
  ListNode<T>* PopFront(ListNode<T> *node) {
    return NULL;
  }
  ListNode<T>* PopBack(ListNode<T> *node) {
    return NULL;
  }
  void Remove(ListNode<T> *node) {
    if (node == head_) {

    }
    if (node == tail_) {

    }
    node->Remove();
  }

 private:
  ListNode<T> *head_;
  ListNode<T> *tail_;
};

}  // namespace utils

#endif // SRC_INCLUDE_COMMON_LIST_HPP_
