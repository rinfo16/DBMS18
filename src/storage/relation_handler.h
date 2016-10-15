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
  void *GetFirst(uint32_t *length);
  void *GetNext(uint32_t *length);
  bool Put(void *tuple, uint32_t length);
  bool Delete(cursor_t cursor);
  bool Create();
  bool Drop();
 private:
  void *GetNextTuple(uint32_t *length);
  Page *page_;
  uint32_t nth_slot_;
  relationid_t relation_id_;
  OpenMode mode_;
  BufferManager *buffer_manager_;
  SpaceManager *space_manager_;

};

}  // namespace storage

#endif //RELATION_HANDLER_INTERFACE_H_
