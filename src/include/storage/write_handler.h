#ifndef WRITE_BATCH_INTERFACE_H_
#define WRITE_BATCH_INTERFACE_H_

#include "common/tuple.h"
#include "io_handler.h"
#include "iterator_handler.h"

namespace storage {

class WriteHandler : public IOHandler {
 public:
  virtual ~WriteHandler() {
  }
  virtual void Put(const TupleWarpper *tuple) = 0;
  virtual void Put(tupleid_t tuple_id, const TupleWarpper *tuple) = 0;
  virtual void Delete(tupleid_t tuple_id) = 0;
  virtual bool Commit() = 0;
};

}

#endif // WRITE_BATCH_INTERFACE_H_
