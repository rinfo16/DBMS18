#ifndef STORAGE_H_
#define STORAGE_H_

#include "storage/storage_service_interface.h"
#include "leveldb/db.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include <memory>
#include <map>
namespace storage {

class LevelStore : public StorageServiceInterface {
 public:
  LevelStore();
  virtual ~LevelStore();
  virtual bool Start();
  virtual void Stop();

  virtual bool CreateRelation(const TableSchema & schema);
  virtual bool CreateIndex(const IndexSchema & index_schema);
  virtual bool DropRelation(const std::string & rel_name);
  virtual IteratorHandler * NewIterator(relationid_t relid);
  virtual WriteHandler * NewWriteHandler(relationid_t relid);
  virtual void DeleteIOObject(IOHandler* handler);
  virtual Relation *GetRelation(const std::string &rel_name);

  void FlushAll();

 private:
  bool InitDB();
  std::string GetDBFilePath(relationid_t id);
  relationid_t GenRelationID();
  void WriteRelMeta(std::auto_ptr<Relation> r);
  bool InsertRelation(std::auto_ptr<Relation> r);
  bool CreateSystemTable();
  bool ReadSystemTable();
  uint32_t NewRelationID();
  std::string path_;
  leveldb::DB* db_class_;
  leveldb::DB* db_attribute_;
  std::map<std::string, Relation*> name_rel_map_;
  std::map<relationid_t, Relation*> id_rel_map_;
  boost::ptr_vector<Relation> all_relations_;
  std::string dbname_;
};


}  // namespace storage

#endif // STORAGE_STORAGE_SERVICE_H_
