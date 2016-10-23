#ifndef SRC_INCLUDE_STORAGE_META_DATA_SERVICE_INTERFACE_H_
#define SRC_INCLUDE_STORAGE_META_DATA_SERVICE_INTERFACE_H_

#include <string>
#include <boost/core/noncopyable.hpp>
#include "common/relation.h"

namespace storage {

class MetaDataServiceInterface : boost::noncopyable {
 public:
  virtual ~MetaDataServiceInterface() {}
  virtual void AddRelation(Relation *rel) = 0;
  virtual Relation* GetRelationByName(const std::string &) = 0;
  virtual Relation* GetRelationByID(relationid_t id) = 0;
  virtual bool RemoveRelationByName(const std::string &) = 0;
  virtual bool RemoveRelationByID(relationid_t id) = 0;
  virtual bool Start() = 0;
  virtual void Stop() = 0;
};

}  // end namespace storage

#endif // SRC_INCLUDE_STORAGE_META_DATA_SERVICE_INTERFACE_H_
