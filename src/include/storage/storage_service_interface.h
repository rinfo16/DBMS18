#ifndef STORAGE_SERVICE_INTERFACE_H_
#define STORAGE_SERVICE_INTERFACE_H_

#include "common/define.h"
#include "storage/relation_handler_interface.h"
namespace storage
{
class StorageServiceInterface
{
public:
  virtual ~StorageServiceInterface();
  virtual RelationHandlerInterface * OpenHandler(relationid_t rel, int mode) = 0;
  virtual void CloseHandler(RelationHandlerInterface* handler) = 0;
};

}
#endif // STORAGE_SERVICE_INTERFACE_H_
