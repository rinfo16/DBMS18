#ifndef EXEC_H__
#define EXEC_H__

#include "common/tuple_row.h"
#include "common/state.h"

namespace executor {
class Exec {
public:
  virtual ~Exec() {}
};

class ExecInterface : public Exec {
public:
  virtual ~ExecInterface() {}
  virtual State Prepare() = 0;
  virtual State Open()  = 0;
  virtual State GetNext(TupleRow *row) = 0;
  virtual void Close() = 0;
};

class CmdInterface : public Exec {
 public:
   virtual ~CmdInterface() {}
   virtual State Prepare() = 0;
   virtual State Exec() = 0;
};
;

}
#endif // EXEC_H__
