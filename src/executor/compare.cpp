#include "executor/compare.h"
namespace executor {
VarcharCompare::VarcharCompare(OperatorType op, ValueExprInterface *left,
                             ValueExprInterface *right)
    : op_(op),
      left_(left),
      right_(right) {
}

bool VarcharCompare::GetValue(TupleRow *row) const {
  char *str;
  uint32_t len;
  str = (char*) left_->GetValue(row, &len);
  std::string str_left(str, len);
  str = (char*) right_->GetValue(row, &len);
  std::string str_right(str, len);
  switch (op_) {
    case kEqual:
      return str_left == str_right;
      break;
    case kLess:
      return str_left < str_right;
      break;
    case kGreater:
      return str_left > str_right;
      break;
    case kLessOrEqual:
      return str_left <= str_right;
      break;
    case kGreaterOrEqual:
      return str_left >= str_right;
      break;
    default:
      break;
  }
  return false;
}

bool VarcharCompare::GetValue(TupleRow *row1, TupleRow *row2) const {
  char *str;
  uint32_t len;
  str = (char*) left_->GetValue(row1, &len);
  std::string str_left(str, len);
  str = (char*) right_->GetValue(row2, &len);
  std::string str_right(str, len);
  switch (op_) {
    case kEqual:
      return str_left == str_right;
      break;
    case kLess:
      return str_left < str_right;
      break;
    case kGreater:
      return str_left > str_right;
      break;
    case kLessOrEqual:
      return str_left <= str_right;
      break;
    case kGreaterOrEqual:
      return str_left >= str_right;
      break;
    default:
      break;
  }
  return false;
}

IntegerCompare::IntegerCompare(OperatorType op, ValueExprInterface *left,
                               ValueExprInterface *right)
    : op_(op),
      left_(left),
      right_(right) {
}

bool IntegerCompare::GetValue(TupleRow *row) const {
  uint32_t len;
  int64_t int_left = *(uint64_t*) left_->GetValue(row, &len);
  int64_t int_right = *(uint64_t*) right_->GetValue(row, &len);
  switch (op_) {
    case kEqual:
      return int_left == int_right;
      break;
    case kLess:
      return int_left < int_right;
      break;
    case kGreater:
      return int_left > int_right;
      break;
    case kLessOrEqual:
      return int_left <= int_right;
      break;
    case kGreaterOrEqual:
      return int_left >= int_right;
      break;
    default:
      break;
  }
  return false;
}

bool IntegerCompare::GetValue(TupleRow *row1, TupleRow *row2) const {
  uint32_t len;
  int64_t int_left = *(uint64_t*) left_->GetValue(row1, &len);
  int64_t int_right = *(uint64_t*) right_->GetValue(row2, &len);
  switch (op_) {
    case kEqual:
      return int_left == int_right;
      break;
    case kLess:
      return int_left < int_right;
      break;
    case kGreater:
      return int_left > int_right;
      break;
    case kLessOrEqual:
      return int_left <= int_right;
      break;
    case kGreaterOrEqual:
      return int_left >= int_right;
      break;
    default:
      break;
  }
  return false;
}

FloatCompare::FloatCompare(OperatorType op, ValueExprInterface *left,
                           ValueExprInterface *right)
    : op_(op),
      left_(left),
      right_(right) {
}

bool FloatCompare::GetValue(TupleRow *row) const {
  uint32_t len;
  double_t float_left = *(double*) left_->GetValue(row, &len);
  double_t float_right = *(double*) right_->GetValue(row, &len);
  switch (op_) {
    case kEqual:
      return float_left == float_right;
      break;
    case kLess:
      return float_left < float_right;
      break;
    case kGreater:
      return float_left > float_right;
      break;
    case kLessOrEqual:
      return float_left <= float_right;
      break;
    case kGreaterOrEqual:
      return float_left >= float_right;
      break;
    default:
      break;
  }
  return false;
}

bool FloatCompare::GetValue(TupleRow *row1, TupleRow *row2) const {
  uint32_t len;
  double_t float_left = *(double*) left_->GetValue(row1, &len);
  double_t float_right = *(double*) right_->GetValue(row2, &len);
  switch (op_) {
    case kEqual:
      return float_left == float_right;
      break;
    case kLess:
      return float_left < float_right;
      break;
    case kGreater:
      return float_left > float_right;
      break;
    case kLessOrEqual:
      return float_left <= float_right;
      break;
    case kGreaterOrEqual:
      return float_left >= float_right;
      break;
    default:
      break;
  }
  return false;
}

}
