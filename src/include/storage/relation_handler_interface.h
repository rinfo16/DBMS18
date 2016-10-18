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
// TODO
// read handler/write handler/create handler/....

class RelationHandlerInterface {
public:
  virtual ~RelationHandlerInterface() {};
  virtual const void *GetFirst(uint32_t *length) = 0;
  virtual const void *GetNext(uint32_t *length) = 0;
  virtual bool Put(void *tuple, uint32_t length) = 0;
  virtual bool Delete(cursor_t tuple) = 0;
  virtual bool Create() = 0;
  virtual bool Drop() = 0;
};

}
#endif // RELATION_HANDLER_H_
