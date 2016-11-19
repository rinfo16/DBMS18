#ifndef SORT_H_
#define SORT_H_

#include "executor/exec_interface.h"
#include "executor/boolean_expr_interface.h"

namespace executor {

class Sort : public ExecInterface {
 public:
  // In Sort executor, we should use two arguments version GetValue:
  //      bool BooleanExprInterface::GetValue(TupleRow *row1, TupleRow *row2)
  // to compare two TupleRow
  Sort(const BooleanExprInterface *tuple_compare);
  virtual ~Sort();
  virtual bool Prepare();
  virtual bool Open();
  virtual bool GetNext(TupleRow *row);
  virtual void Close();
};

}  // namespace executor

#endif // SORT_H_
