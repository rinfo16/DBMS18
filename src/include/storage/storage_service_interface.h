#ifndef STORAGE_SERVICE_INTERFACE_H_
#define STORAGE_SERVICE_INTERFACE_H_

#include "common/define.h"
#include "storage/iterator_interface.h"
#include <boost/core/noncopyable.hpp>
#include "storage/write_batch_interface.h"
#include "storage/meta_data_manager.h"
#include "common/table_schema.h"

namespace storage {


class StorageServiceInterface : public boost::noncopyable {
 public:
  virtual ~StorageServiceInterface(){};
  virtual bool Start() = 0;
  virtual bool CreateRelation(const TableSchema & schema) = 0;
  virtual bool DropRelation(const std::string & rel_name) = 0;
  virtual IteratorInterface * NewIterator(const std::string & rel_name) = 0;
  virtual WriteBatchInterface * NewWriteBatch(const std::string & rel_name) = 0;
  virtual void DeleteIOObject(IOObjectInterface* io_object) = 0;
  virtual MetaDataManagerInterface *GetMetaDataManager() = 0;

  static StorageServiceInterface* Instance();
  static bool Load(const std::string & table_name, const std::string & file_path);
};

}
#endif // STORAGE_SERVICE_INTERFACE_H_
