#ifndef SRC_INCLUDE_COMMON_LIST_HPP_
#define SRC_INCLUDE_COMMON_LIST_HPP_

#include "common/define.h"
#include <assert.h>

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
    next_ = right_node;
    right_node->prev_ = this;
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
    if (prev_ != NULL) {
      prev_->next_ = next_;
    }
    if (next_ != NULL) {
      next_->prev = prev_;
    }
  }

  ListNode<T>* Next() {
    return next_;
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
    if (tail_ == NULL) {
      assert(head_ == NULL);
      head_ = node;
      tail_ = node;
    } else {
      tail_->LinkToRight(node);
      tail_ = node;
    }
  }
  void PushFront(ListNode<T> *node) {
  }
  ListNode<T>* PopFront() {
    ListNode<T> *node = head_;
    if (head_ == tail_) {
      assert(head_ == NULL || head_->Next() == NULL);
      assert(tail_ == NULL || tail_->Prev() == NULL);
      head_ = NULL;
      tail_ = NULL;
    } else {
      head_ = head_->Next();
    }

    return node;
  }

  ListNode<T>* PopBack() {
    return NULL;
  }

  void Remove(ListNode<T> *node) {
    if (node == head_) {
      head_ = node->Next();
    }
    if (node == tail_) {
      tail_ = node->Prev();
    }
    node->Remove();
  }

 private:
  ListNode<T> *head_;
  ListNode<T> *tail_;
};

}  // namespace utils

#endif // SRC_INCLUDE_COMMON_LIST_HPP_
