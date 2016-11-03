#ifndef TUPLE_ROW_H__
#define TUPLE_ROW_H__

#include "common/define.h"
#include "common/tuple.h"

class TupleRow {
 public:
  TupleRow() {
    tuples_ = new Tuple*[1];
    size_ = 1;
  }

  TupleRow(uint32_t n) {
    tuples_ = new Tuple*[n];
    size_ = n;
  }

  void SetTuple(uint32_t n, Tuple *t) {
    tuples_[n] = t;
  }

  Tuple *GetTuple(uint32_t n) {
    return tuples_[n];
  }

  const Tuple *GetTuple(uint32_t n) const {
    return tuples_[n];
  }

  uint32_t Size() const {
    return size_;
  }

 private:
  Tuple **tuples_;
  uint32_t size_;
};

#endif // TUPLE_ROW_H__
