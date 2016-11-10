#ifndef DATA_TYPE_H_
#define DATA_TYPE_H_

#include <string>

#include "postgres/pg_type.h"
#include "common/define.h"

enum DataType {
  kDTInvalid = 0,
  kDTInteger = INT8OID,
  kDTFloat = FLOAT8OID,
  kDTDate = DATEOID,
  kDTVarchar = VARCHAROID
};


union DataValue {
  int64_t integer_;
  double_t float_;
  int64_t date_;
};

#endif // DATA_TYPE_H_
