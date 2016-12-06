#ifndef SRC_STORAGE_WRITE_BATCH_H_
#define SRC_STORAGE_WRITE_BATCH_H_

#include "storage/write_batch_interface.h"
#include "space_manager.h"

namespace storage {

class WriteBatch : public WriteBatchInterface {
 public:
  WriteBatch(const PageID & id, SpaceManager *space_manager);
  virtual ~WriteBatch();
  virtual bool Put(TupleWarpper *tuple);
 private:
  SpaceManager *space_manager_;
  PageID pageid_;
};

}  // namespace storage

#endif // SRC_STORAGE_WRITE_BATCH_H_
