#include "parser/expression.h"
namespace ast {
ptree Expression::ToPropertyTree() const {
  ptree expr;
  std::string op;
  switch (operator_) {
    case kAssign:
    case kEqual:
      op = "=";
      break;
    case kLess:
      op = "<";
      break;
    case kGreater:
      op = ">";
      break;
    case kLessOrEqual:
      op = "<=";
      break;
    case kGreaterOrEaual:
      op = ">=";
      break;
    default:
      break;
  }
  if (!op.empty()) {
    expr.put(STR_OPERATOR_TYPE, op);
  }
  if (left_) {
    expr.put_child(STR_LEFT, left_->ToPropertyTree());
  }
  if (right_) {
    expr.put_child(STR_RIGHT, right_->ToPropertyTree());
  }
  return expr;
}
}
