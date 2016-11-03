#ifndef TUPLE_DESC_H_
#define TUPLE_DESC_H_

#include <vector>
#include "common/slot.h"
#include "common/data_type.h"

struct ItemSlot {
  uint32_t nth_tuple_;
  uint32_t nth_item_;
};

struct ColumnDesc {
  std::string column_name_;
  DataType data_type_;
  int16_t data_type_size_;
  relationid_t relation_id_;
  attributeid_t attribute_id_;
  ItemSlot item_slot_;
};

class RowDesc {
 public:
  RowDesc()
      : column_count_(0){
  }
  ~RowDesc() {
  }

  void PushColumnDesc(const ColumnDesc & column_desc) {
    all_columns_desc_.push_back(column_desc);
  }

  const ColumnDesc & GetColumnDesc(uint32_t n) const {
    return all_columns_desc_[n];
  }

  size_t GetColumnCount() const {
    return all_columns_desc_.size();
  }
private:
  std::vector<uint32_t> mapping_;
  uint32_t column_count_;
  std::vector<ColumnDesc> all_columns_desc_;
};

#endif // TUPLE_DESC_H_
