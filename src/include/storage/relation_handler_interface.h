#ifndef RELATION_HANDLER_H_
#define RELATION_HANDLER_H_

#include "common/define.h"

namespace storage {

enum OpenMode {
  kRelationRead,
  kRelationWrite,
  kRelationDrop,
  kRelationCreate
};

class RelationHandlerInterface {
public:
  virtual ~RelationHandlerInterface() {};
  virtual cursor_t BeginRead() = 0;
  virtual void *ReadNext(cursor_t *tuple, uint32_t *length) = 0;
  virtual void *ReadRrev(cursor_t *tuple, uint32_t *length) = 0;
  virtual bool Delete(cursor_t tuple) = 0;
  virtual bool Write(void *tuple, uint32_t length) = 0;
  virtual bool Create() = 0;
  virtual bool Drop() = 0;
};

}
#endif // RELATION_HANDLER_H_
