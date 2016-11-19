#include "common/define.h"

bool IsArithmeticOperator(OperatorType type) {
  return type > kArithmeticBegin && type < kArithmeticEnd;
}

bool IsBooleanOperator(OperatorType type) {
  return type > kBooleanBegin && type < kBooleanEnd;
}

bool IsLogicOperator(OperatorType type) {
  return type > kLogicBegin && type < kLogicEnd;
}

bool IsCompareOperator(OperatorType type) {
  return type > kCompareBegin && type < kCompareEnd;
}
