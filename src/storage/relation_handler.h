#ifndef RELATION_HANDLER_INTERFACE_H_
#define RELATION_HANDLER_INTERFACE_H_

#include "storage/relation_handler_interface.h"

namespace storage {

class RelationHandler : public RelationHandlerInterface {
 public:
  cursor_t BeginRead();
  void *ReadNext(cursor_t *cursor, uint32_t *length);
  void *ReadRrev(cursor_t *cursor, uint32_t *length);
  bool Delete(cursor_t cursor) = 0;
  bool Write(void *tuple, uint32_t length);
  bool Drop();
};

}  // namespace storage

#endif //RELATION_HANDLER_INTERFACE_H_
