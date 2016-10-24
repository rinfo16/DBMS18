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
  virtual bool CreateRelation(const TableSchema & schema);
  virtual bool DropRelation(const std::string & rel_name);
  virtual IteratorInterface * NewIterator(const std::string & rel_name);
  virtual WriteBatchInterface * NewWriteBatch(const std::string & rel_name);
  virtual void DeleteIOObject(IOObjectInterface* io_object);

  void FlushAll();

  void InitDB();

  MetaDataManagerInterface *GetMetaDataManager() { return meta_data_manager_; }
 private:
  BufferManager *buffer_manager_;
  SpaceManager *space_manager_;
  MetaDataManagerInterface *meta_data_manager_;
};

typedef utils::Singleton<storage::StorageService> Storage;

}  // namespace storage

#endif // STORAGE_STORAGE_SERVICE_H_
