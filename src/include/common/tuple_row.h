#ifndef TUPLE_ROW_H__
#define TUPLE_ROW_H__

#include "common/define.h"
#include "common/tuple.h"

class TupleRow {
 public:
  TupleRow() {
    tuples_.resize(1);
  }

  TupleRow(uint32_t n) {
    tuples_.resize(n);
  }

  void CopyTo(TupleRow *dest_row) {
    if (tuples_.size() <= dest_row->tuples_.size()) {
      for (auto i = 0; i < tuples_.size(); i++) {
       dest_row->tuples_[i] = tuples_[i];
      }
    }
  }

  void SetTuple(uint32_t n, Tuple t) {
    tuples_[n] = t;
  }

  Tuple GetTuple(uint32_t n) {
    return tuples_[n];
  }

  const Tuple GetTuple(uint32_t n) const {
    return tuples_[n];
  }

  uint32_t Size() const {
    return tuples_.size();
  }

 private:
  std::vector<Tuple> tuples_;
};

#endif // TUPLE_ROW_H__
