#include "executor/const_value.h"

const char *VarcharConstValue::GetValue(TupleRow *row, uint32_t *length) const {
  *length = value_.size();
  return value_.c_str();
}

const char *IntegerConstValue::GetValue(TupleRow *row, uint32_t *length) const {
  *length = sizeof(value_);
  return (char*) &value_;
}

const char *FloatConstValue::GetValue(TupleRow *row, uint32_t *length) const {
  *length = sizeof(value_);
  return (char*) &value_;
}

