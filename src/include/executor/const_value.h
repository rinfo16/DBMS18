#ifndef CONST_VALUE_H__
#define CONST_VALUE_H__

#include "executor/value_expr_interface.h"

class VarcharConstValue : public ValueExprInterface {
public:
  ValcharConstValue(const std::string & value) : value_(value){

  }
  virtual const char *GetValue(TupleRow *row, uint32_t *length);
private:
  std::string value_;
}


class IntegerConstValue : public ValueExprInterface {
 public:
  IntegerConstValue(double_t & value) : value_(value){
  }
  virtual const char *GetValue(TupleRow *row, uint32_t *length);
 private:
  int64_t value_;
}

class FloatConstValue : public ValueExprInterface {
 public:
  FloatConstValue(double_t & value) : value_(value){

  }
  virtual const char *GetValue(TupleRow *row, uint32_t *length);
 private:
  double_t value_;
}

#endif // CONST_VALUE_H__
