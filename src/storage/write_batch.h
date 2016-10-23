#ifndef SRC_STORAGE_WRITE_BATCH_H_
#define SRC_STORAGE_WRITE_BATCH_H_

#include "storage/write_batch_interface.h"
#include "relation_handler_interface.h"


namespace storage {

class WriteBatch : public WriteBatchInterface {
 public:
  WriteBatch(RelationHandlerInterface *rel_handler);
  virtual ~WriteBatch();
  virtual bool Put(TupleWarpper *tuple);
 private:
  RelationHandlerInterface *rel_handler_;
};

}  // namespace storage

#endif // SRC_STORAGE_WRITE_BATCH_H_
