#include "write_batch.h"

namespace storage {

WriteBatch::WriteBatch(relationid_t rel_id, SpaceManager *space_manager)
    : rel_id_(rel_id),
      space_manager_(space_manager) {
}

WriteBatch::~WriteBatch() {

}

bool WriteBatch::Put(TupleWarpper *tuple) {
  PageID segment_header_pageid;
  segment_header_pageid.fileno_ = 0;
  segment_header_pageid.pageno_ = rel_id_;
  return space_manager_->WriteTuple(segment_header_pageid,
                                    (void*) tuple->Data(),
                                    (uint32_t) tuple->Size());
}

}  // namespace storage
