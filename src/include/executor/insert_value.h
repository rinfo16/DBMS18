#ifndef INSERT_VALUE_H__
#define INSERT_VALUE_H__

#include "storage/write_handler.h"
#include "common/tuple.h"
#include "executor/exec_interface.h"

namespace executor {

class InsertValue : public CmdInterface {
 public:
  InsertValue(storage::WriteHandler *handler, Tuple tuple, size_t size);
  virtual State Prepare();
  virtual State Exec();
 private:
  storage::WriteHandler *handler_;
  Tuple tuple_;
  size_t size_;
};
}  // namespace executor

#endif // INSERT_VALUE_H__
