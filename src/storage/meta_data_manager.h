#ifndef SRC_STORAGE_META_DATA_MANAGER_H_
#define SRC_STORAGE_META_DATA_MANAGER_H_

#include "storage/meta_data_manager_interface.h"
#include <vector>
#include <map>
#include "buffer_manager.h"
#include "space_manager.h"

namespace storage {

class MetaDataManager : public MetaDataManagerInterface {
 public:
  MetaDataManager(BufferManager *buffer_manager, SpaceManager *space_manager);
  virtual ~MetaDataManager();
  virtual void AddRelation(Relation *rel);
  virtual Relation* GetRelationByName(const std::string &);
  virtual Relation* GetRelationByID(relationid_t id);
  virtual bool RemoveRelationByName(const std::string &);
  virtual bool RemoveRelationByID(relationid_t id);
  virtual bool Start();
  virtual void Stop();
 private:
  MetaDataManager();
  void ReadSysTable();
  void CreateSysTable();
  bool InsertRelation(Relation *rel);
  std::vector<Relation*> all_relations_;
  std::map<relationid_t, Relation*> id_rel_map_;
  std::map<std::string, Relation*> name_rel_map_;
  std::string data_directory_;
  storage::BufferManager *buffer_manager_;
  storage::SpaceManager *space_manager_;
};

}  // namespace storage

#endif // SRC_STORAGE_META_DATA_MANAGER_H_
