#ifndef TUPLE_DESC_H_
#define TUPLE_DESC_H_

#include <vector>
#include "common/slot.h"
#include "common/data_type.h"

class TupleDesc {
 public:
  TupleDesc()
      : column_count_(0){
  }
  ~TupleDesc() {
  }

  DataType GetType(uint32_t n) { return data_type_[mapping_[n]]; }

  std::vector<DataType> data_type_;
  std::vector<uint32_t> mapping_;
  uint32_t column_count_;
};

#endif // TUPLE_DESC_H_
