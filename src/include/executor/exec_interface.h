#ifndef EXEC_H__
#define EXEC_H__

#include "common/tuple_row.h"

namespace executor {
class ExecInterface {
public:
  virtual ~ExecInterface() {}
  virtual bool Prepare() = 0;
  virtual bool Open()  = 0;
  virtual bool GetNext(TupleRow *row) = 0;
  virtual void Close() = 0;
private:
};

}
#endif // EXEC_H__
