#ifndef STORAGE_H_
#define STORAGE_H_


#include <boost/ptr_container/ptr_vector.hpp>
#include <memory>
#include <map>
#include <mutex>
#include "storage/storage_service_interface.h"
#include "leveldb/db.h"

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
 private:
  bool InitDB();

  void FlushAll();

  std::string GetDBFilePath(relationid_t id);
  relationid_t GenRelationID();
  void WriteRelMeta(const Relation *r);
  bool InsertRelation(Relation *r);
  bool CreateSystemTable();
  bool ReadSystemTable();
  uint32_t NewRelationID();

  std::string path_;
  leveldb::DB* db_class_;
  leveldb::DB* db_attribute_;
  std::map<std::string, Relation*> name_rel_map_;
  std::map<relationid_t, Relation*> id_rel_map_;
  boost::ptr_vector<Relation> all_relations_;
  std::mutex write_map_mutex_;
  std::mutex iterator_map_mutex_;
  std::map<relationid_t, WriteHandler*> write_handler_map_;
  std::map<relationid_t, IteratorHandler*> iterator_handler_map_;
  std::string dbname_;
};


}  // namespace storage

#endif // STORAGE_STORAGE_SERVICE_H_
