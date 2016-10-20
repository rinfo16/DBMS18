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

//judge if node is the first one
  bool IsFirst() const {
      return false;
  }

//judge if the node is the last one
  bool IsLast() const {
    return false;
  }

//insert node on the right end
  void LinkToRight(ListNode<T> *right_node) {
    next_ = right_node;
    right_node->prev_ = this;

  }




//insert node on the left end
  void LinkToLeft(ListNode <T> *left_node) {
    left_node->next_=this;
    this->prev_=left_node;
  }

//return the first node
  ListNode<T>* First() {
    ListNode <T> * p;
    p=this;
    while (p->prev_!=NULL)
    {
       p=p->prev_;
     }
     return p;
  }

//return the last node
  ListNode<T>* Last() {
   ListNode <T> * p;
   while (p->next_!=NULL)
     {
       p=p->next_;
     }
     return p;
  }

//delete one node
  void Remove() {
    if (prev_ != NULL) {
      prev_->next_ = next_;
    }
    if (next_ != NULL) {
      next_->prev = prev_;
    }

  }


//return the next node

  ListNode<T>* Next() {
    return next_;

  }

//return the prev node
  ListNode <T> * Prev(){
    return this->prev_;
  }

 private:
  ListNode <T> *prev_;
  ListNode <T> *next_;
};

template <typename T>
class List:public T {
 public:
  List()
       :head_(NULL),
        tail_(NULL) {
    }

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
    if (head_ == NULL) {
      assert(tail_ == NULL);
      tail_ = node;
      head_ = node;
    } else {
      head_->LinkToLeft(node);
      head_ = node;
    }
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
    ListNode <T> * node =tail_;
    if (head_==tail_){
      assert(tail_==NULL || tail_->Prev()==NULL);
      assret(head_==NULL || head_->Next()==NULL);
      tail_=NULL;
      head_=NULL;
    }
    else{
      tail_=tail_->prev_;
    }

    return node;
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
  ListNode <T> * head_;
  ListNode <T> * tail_;
};
}  // namespace utils

#endif // SRC_INCLUDE_COMMON_LIST_HPP_
