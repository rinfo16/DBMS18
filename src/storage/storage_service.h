#ifndef STORAGE_STORAGE_SERVICE_H_
#define STORAGE_STORAGE_SERVICE_H_

#include "storage/storage_service_interface.h"
#include "common/singleton.hpp"
namespace storage {

class BufferManager;
class SpaceManager;

class StorageService : public StorageServiceInterface {
 public:
  StorageService();
  virtual ~StorageService();
  virtual bool Start();
  void Stop();
  virtual bool CreateRelation(relationid_t rel);
  virtual bool DropRelation(relationid_t rel);
  virtual IteratorInterface * NewIterator(relationid_t rel);
  virtual WriteBatchInterface * NewWriteBatch(relationid_t rel);
  virtual void DeleteIOObject(IOObjectInterface* io_object);

  void FlushAll();

  void InitDB();
 private:
  BufferManager *buffer_manager_;
  SpaceManager *space_manager_;
};

typedef utils::Singleton<storage::StorageService> Storage;

}  // namespace storage

#endif // STORAGE_STORAGE_SERVICE_H_
