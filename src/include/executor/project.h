#ifndef PROJECT_H__
#define PROJECT_H__

#include "executor/exec_interface.h"
#include "executor/boolean_expr_interface.h"
#include "executor/value_expr_interface.h"
#include "executor/sort.h"

namespace executor {

class Project : public ExecInterface {
 public:
  Project(ExecInterface *left,
          const std::vector<ValueExprInterface*> & project_attr,
          int32_t tuple_count);
  virtual ~Project();
  virtual State Prepare();
  virtual State Open();
  virtual State GetNext(TupleRow *row);
  virtual void Close();
 private:
  ExecInterface *left_;
  TupleRow *tuple_row_;
  const std::vector<ValueExprInterface*> & project_attr_;
};

}  // namespace executor
#endif // PROJECT_H__
