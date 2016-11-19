#ifndef MERGE_JOIN_H__
#define MERGE_JOIN_H__

#include "executor/exec_interface.h"
#include "executor/boolean_expr_interface.h"
#include "executor/sort.h"

namespace executor {

class MergeJoin : public ExecInterface {
 public:
  MergeJoin(Sort *left, Sort *right,
                 const BooleanExprInterface *join_predicate);
  virtual ~MergeJoin();
  virtual bool Prepare();
  virtual bool Open();
  virtual bool GetNext(TupleRow *row);
  virtual void Close();
};

}  // namespace executor

#endif // MERGE_JOIN_H__
