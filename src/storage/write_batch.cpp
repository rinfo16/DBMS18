#include "write_batch.h"

namespace storage {

WriteBatch::WriteBatch(const PageID & id, SpaceManager *space_manager)
    : pageid_(id),
      space_manager_(space_manager) {
}

WriteBatch::~WriteBatch() {

}

bool WriteBatch::Put(TupleWarpper *tuple) {
  return space_manager_->WriteTuple(pageid_,
                                    (void*) tuple->Data(),
                                    (uint32_t) tuple->Size());
}

}  // namespace storage
