#ifndef LOGIC_EXPR_H_
#define LOGIC_EXPR_H_

#include "executor/boolean_expr_interface.h"

namespace executor {

class LogicExpr : public BooleanExprInterface {
 public:
  LogicExpr(OperatorType op, BooleanExprInterface *left,
            BooleanExprInterface *right);
  virtual ~LogicExpr();
  virtual bool GetValue(TupleRow *row) const;
  virtual bool GetValue(TupleRow *row1, TupleRow *row2) const;
 private:
  OperatorType op_;
  BooleanExprInterface *left_;
  BooleanExprInterface *right_;
};

}  // namespace executor

#endif // LOGIC_EXPR_H_
