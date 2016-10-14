#ifndef RELATION_HANDLER_INTERFACE_H_
#define RELATION_HANDLER_INTERFACE_H_

#include "storage/relation_handler_interface.h"
#include "storage/space_manager.h"
#include "storage/buffer_manager.h"

namespace storage {

class RelationHandler : public RelationHandlerInterface {
 public:
  RelationHandler(relationid_t id, OpenMode mode, BufferManager *buffer_manager,
                  SpaceManager *space_manager);
  cursor_t BeginRead();
  void *ReadNext(cursor_t *cursor, uint32_t *length);
  void *ReadRrev(cursor_t *cursor, uint32_t *length);
  bool Delete(cursor_t cursor);
  bool Write(void *tuple, uint32_t length);
  bool Create();
  bool Drop();
 private:
  relationid_t relation_id_;
  OpenMode mode_;
  BufferManager *buffer_manager_;
  SpaceManager *space_manager_;

};

}  // namespace storage

#endif //RELATION_HANDLER_INTERFACE_H_
