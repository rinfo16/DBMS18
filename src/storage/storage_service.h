#ifndef STORAGE_STORAGE_SERVICE_H_
#define STORAGE_STORAGE_SERVICE_H_

#include "storage/storage_service_interface.h"

namespace storage {

class StorageService : public StorageServiceInterface {
 public:
  StorageService();
  virtual ~StorageService();
  virtual RelationHandlerInterface * OpenHandler(relationid_t rel, int mode);
  virtual void CloseHandler(RelationHandlerInterface* handler);
};

}  // namespace storage

#endif // STORAGE_STORAGE_SERVICE_H_
