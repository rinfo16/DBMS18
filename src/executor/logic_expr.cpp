#include "executor/logic_expr.h"
#include "common/define.h"
#include <assert.h>

namespace executor {

LogicExpr::LogicExpr(OperatorType op, BooleanExprInterface *left,
                     BooleanExprInterface *right)
    : op_(op),
      left_(left),
      right_(right) {
}

LogicExpr::~LogicExpr() {
}

bool LogicExpr::GetValue(TupleRow *row) const {
  if (op_ == kAnd) {
    return left_->GetValue(row) and right_->GetValue(row);
  } else if (op_ == kOr) {
    return left_->GetValue(row) or right_->GetValue(row);
  } else if (op_ == kXor) {
    return left_->GetValue(row) xor right_->GetValue(row);
  }
  assert(false);
  return false;
}

bool LogicExpr::GetValue(TupleRow *row1, TupleRow *row2) const {
  if (op_ == kAnd) {
    return left_->GetValue(row1) and right_->GetValue(row2);
  } else if (op_ == kOr) {
    return left_->GetValue(row1) or right_->GetValue(row2);
  } else if (op_ == kXor) {
    return left_->GetValue(row1) xor right_->GetValue(row2);
  }
  assert(false);
  return false;
}

}  // namespace executor
