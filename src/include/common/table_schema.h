#ifndef SRC_INCLUDE_COMMON_TABLE_SCHEMA_H_
#define SRC_INCLUDE_COMMON_TABLE_SCHEMA_H_

#include <vector>
#include <string>
#include "common/data_type.h"

struct ColumnSchema {
  std::string name_;
  DataType data_type_;
  int length_;
  bool is_null_;
};

struct TableSchema {
  std::string name_;
  std::vector<ColumnSchema> column_list_;
};

struct IndexSchema {
  std::string relation_name_;
  std::string attribute_name_;
  IndexType index_type_;
};

#endif // SRC_INCLUDE_COMMON_TABLE_SCHEMA_H_
