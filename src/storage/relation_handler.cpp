#include "relation_handler.h"

namespace storage {

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

}   // namespace storage
