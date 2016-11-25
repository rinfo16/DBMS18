#ifndef SORT_H_
#define SORT_H_

#include "exec_interface.h"
#include "executor/boolean_expr_interface.h"

namespace executor {

class Sort : public ExecInterface {
 public:
  // In Sort executor, we should use two arguments version GetValue:
  //      bool BooleanExprInterface::GetValue(TupleRow *row1, TupleRow *row2)
  // to compare two TupleRow
  Sort(const BooleanExprInterface *tuple_compare);
  virtual ~Sort();
  virtual State Prepare();
  virtual State Open();
  virtual State GetNext(TupleRow *row);
  virtual void Close();
};

}  // namespace executor

#endif // SORT_H_
