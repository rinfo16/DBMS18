#ifndef NESTED_LOOP_JOIN_H__
#define NESTED_LOOP_JOIN_H__

#include "executor/exec_interface.h"
#include "executor/boolean_expr_interface.h"
#include "common/tuple_row.h"

namespace executor {

class NestedLoopJoin : public ExecInterface {
 public:
  NestedLoopJoin(ExecInterface *left, ExecInterface *right,
                 const BooleanExprInterface *join_predicate);
  virtual ~NestedLoopJoin();
  virtual State Prepare();
  virtual State Open();
  virtual State GetNext(TupleRow *row);
  virtual void Close();
};

}  // namespace executor

#endif // NESTED_LOOP_JOIN_H__
