#ifndef OPERATION_H__
#define OPERATION_H__

#include "parser/ast_def.h"
#include "executor/value_expr_interface.h"
#include "executor/boolean_expr_interface.h"
namespace executor {
class StringCompare : public BooleanExprInterface {
 public:
  StringCompare(OperatorType op, ValueExprInterface *left,
                ValueExprInterface *right);
  virtual bool GetValue(TupleRow *row) const;
 private:
  OperatorType op_;
  ValueExprInterface *left_;
  ValueExprInterface *right_;
};

class IntegerCompare : public BooleanExprInterface {
 public:
  IntegerCompare(OperatorType op, ValueExprInterface *left,
                 ValueExprInterface *right);
  virtual bool GetValue(TupleRow *row) const;
 private:
  OperatorType op_;
  ValueExprInterface *left_;
  ValueExprInterface *right_;
};

class FloatCompare : public BooleanExprInterface {
 public:
  FloatCompare(OperatorType op, ValueExprInterface *left,
               ValueExprInterface *right);
  virtual bool GetValue(TupleRow *row) const;
 private:
  OperatorType op_;
  ValueExprInterface *left_;
  ValueExprInterface *right_;
};
}
#endif // OPERATION_H__
