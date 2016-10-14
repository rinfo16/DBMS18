#include "relation_handler.h"

namespace storage {

RelationHandler::RelationHandler(relationid_t relation_id, OpenMode mode,
                                 BufferManager *buffer_manager,
                                 SpaceManager *space_manager) {
  relation_id_ = relation_id;
  mode_ = mode;
  buffer_manager_ = buffer_manager;
  space_manager_ = space_manager;

}

cursor_t RelationHandler::BeginRead() {
  return 0;
}
void *RelationHandler::ReadNext(cursor_t *cursor, uint32_t *length) {
  return NULL;
}
void *RelationHandler::ReadRrev(cursor_t *cursor, uint32_t *length) {
  return NULL;
}
bool RelationHandler::Delete(cursor_t cursor) {
  return false;
}
bool RelationHandler::Write(void *tuple, uint32_t length) {
  return false;
}
bool RelationHandler::Drop() {
  return false;
}

bool RelationHandler::Create() {
  if (mode_ != kRelationCreate)
    return false;
  PageID id;
  id.blockno_ = relation_id_;
  // TODO  read/write meta data ...
  return space_manager_->CreateSegment(&id);
}
}   // namespace storage
