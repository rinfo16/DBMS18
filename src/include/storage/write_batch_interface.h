#ifndef WRITE_BATCH_INTERFACE_H_
#define WRITE_BATCH_INTERFACE_H_

#include "common/tuple.h"
#include "storage/iterator_interface.h"
#include "storage/io_object_interface.h"

namespace storage {

class WriteBatchInterface : public IOObjectInterface {
 public:
  virtual ~WriteBatchInterface(){};
  virtual bool Put(TupleWarpper *tuple) = 0;
};

}

#endif // WRITE_BATCH_INTERFACE_H_
