#ifndef STORAGE_STORAGE_SERVICE_H_
#define STORAGE_STORAGE_SERVICE_H_

#include "storage/storage_service_interface.h"
#include "common/singleton.hpp"
namespace storage {

class BufferManager;
class SpaceManager;

class StorageService : public StorageServiceInterface, public utils::Singleton<
    StorageService> {
 public:
  StorageService();
  virtual ~StorageService();
  virtual bool Start();
  virtual RelationHandlerInterface * OpenHandler(relationid_t rel,
                                                 OpenMode mode);
  virtual void CloseHandler(RelationHandlerInterface* handler);

  void InitDB();
  void FlushAll();
  void Stop();
 private:
  BufferManager *buffer_manager_;
  SpaceManager *space_manager_;
};

}  // namespace storage

#endif // STORAGE_STORAGE_SERVICE_H_
