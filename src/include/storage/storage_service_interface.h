#ifndef STORAGE_SERVICE_INTERFACE_H_
#define STORAGE_SERVICE_INTERFACE_H_

#include "common/define.h"
#include "storage/iterator_interface.h"
#include <boost/core/noncopyable.hpp>
#include "storage/write_batch_interface.h"
#include "storage/meta_data_manager.h"
#include "common/relation.h"

namespace storage {


class StorageServiceInterface : public boost::noncopyable {
 public:
  virtual ~StorageServiceInterface(){};
  virtual bool Start() = 0;
  // TODO .. this function should put a relation define class as a parameter
  virtual bool CreateRelation(Relation *rel) = 0;
  virtual bool DropRelation(relationid_t rel) = 0;
  virtual IteratorInterface * NewIterator(relationid_t rel) = 0;
  virtual WriteBatchInterface * NewWriteBatch(relationid_t rel) = 0;
  virtual void DeleteIOObject(IOObjectInterface* io_object) = 0;
  virtual MetaDataManagerInterface *GetMetaDataManager() = 0;
};

}
#endif // STORAGE_SERVICE_INTERFACE_H_
