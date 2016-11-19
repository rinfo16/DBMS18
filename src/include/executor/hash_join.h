#ifndef HASH_JOIN_H__
#define HASH_JOIN_H__

#include "executor/exec_interface.h"
#include "executor/boolean_expr_interface.h"
#include "executor/hash.h"

namespace executor {

class HashJoin : public ExecInterface {
 public:
  HashJoin(Hash *left, Hash *right, const BooleanExprInterface *join_predicate);
  virtual ~HashJoin();
  virtual State Prepare();
  virtual State Open();
  virtual State GetNext(TupleRow *row);
  virtual void Close();
};

}  // namespace executor

#endif // HASH_JOIN_H__
