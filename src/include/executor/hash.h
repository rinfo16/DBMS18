#ifndef EXECUTOR_HASH_H_
#define EXECUTOR_HASH_H_

#include "executor/exec_interface.h"
#include "executor/boolean_expr_interface.h"
#include "executor/value_expr_interface.h"
namespace executor {

class Bucket : public ExecInterface {
 public:
  Bucket();
  virtual ~Bucket();
  virtual State Prepare();
  virtual State Open();
  virtual State GetNext(TupleRow *row);
  virtual void Close();
};

class Hash : public ExecInterface {
 public:
  Hash(uint32_t bucket_count,
       ExecInterface *left,
       const std::vector<ValueExprInterface*> & hash_by_att);
  virtual ~Hash();
  virtual State Prepare();
  virtual State Open();
  virtual State GetNext(TupleRow *row);
  virtual void Close();

  Bucket *GetFirstBucket();
  Bucket *GetNextBucket();
};

}  // namespace executor

#endif // EXECUTOR_HASH_H_
