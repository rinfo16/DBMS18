#ifndef EXEC_H__
#define EXEC_H__

#include "common/tuple_row.h"
#include "common/state.h"

namespace executor {
class ExecInterface {
public:
  virtual ~ExecInterface() {}
  virtual State Prepare() = 0;
  virtual State Open()  = 0;
  virtual State GetNext(TupleRow *row) = 0;
  virtual void Close() = 0;
private:
};

}
#endif // EXEC_H__
