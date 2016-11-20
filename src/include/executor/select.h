#ifndef EXECUTOR_SELECT_H__
#define EXECUTOR_SELECT_H__

#include "executor/exec_interface.h"
#include "executor/boolean_expr_interface.h"

namespace executor {

class Select : public ExecInterface {
 public:
  Select(ExecInterface *left, const BooleanExprInterface *predicate);
  virtual ~Select();
  virtual State Prepare();
  virtual State Open();
  virtual State GetNext(TupleRow *row);
  virtual void Close();
 private:
  Select();
  ExecInterface *left_;
  const BooleanExprInterface *predicate_;
};
} // namespace executor

#endif // EXECUTOR_SELECT_H__
