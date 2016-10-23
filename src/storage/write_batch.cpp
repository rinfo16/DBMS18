#include "write_batch.h"

namespace storage {

WriteBatch::WriteBatch(RelationHandlerInterface *rel_handler):rel_handler_(rel_handler) {
  // TODO Auto-generated constructor stub

}

WriteBatch::~WriteBatch() {
  delete rel_handler_;
}

bool WriteBatch::Put(Tuple *tuple) {
  return rel_handler_->Put((void*)tuple->Data(), (uint32_t)tuple->Size());
}

}  // namespace storage
