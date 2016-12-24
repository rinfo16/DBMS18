#ifndef STORAGE_SERVICE_INTERFACE_H_
#define STORAGE_SERVICE_INTERFACE_H_

#include <boost/utility.hpp>
#include "common/define.h"
#include "common/table_schema.h"
#include "common/relation.h"
#include "iterator_handler.h"
#include "write_handler.h"

namespace storage {


class StorageServiceInterface : public boost::noncopyable {
 public:
  virtual ~StorageServiceInterface(){};
  virtual bool Start() = 0;
  virtual void Stop() = 0;

  virtual bool CreateRelation(const TableSchema & schema) = 0;
  virtual bool CreateIndex(const IndexSchema & index_schema) = 0;
  virtual bool DropRelation(const std::string & rel_name) = 0;
  virtual IteratorHandler * NewIterator(relationid_t relid) = 0;
  virtual WriteHandler * NewWriteHandler(relationid_t relid) = 0;
  virtual void DeleteIOObject(IOHandler* handler) = 0;
  virtual Relation *GetRelation(const std::string &rel_name) = 0;
  static StorageServiceInterface* Instance();
};

}
#endif // STORAGE_SERVICE_INTERFACE_H_
