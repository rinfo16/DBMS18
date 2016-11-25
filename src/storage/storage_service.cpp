#include "storage_service.h"
#include "write_batch.h"
#include "iterator.h"
#include "meta_data_manager.h"
#include "storage/loader.h"

namespace storage {

StorageService::StorageService() {
  buffer_manager_ = new BufferManager();
  space_manager_ = new SpaceManager(buffer_manager_);
  meta_data_manager_ = new MetaDataManager(buffer_manager_, space_manager_);
}

StorageService::~StorageService() {
  delete space_manager_;
  delete buffer_manager_;
  delete meta_data_manager_;
}

bool StorageService::Start() {
  space_manager_->InitDB();

  if (!buffer_manager_->Start()) {
    return false;
  }

  if (!meta_data_manager_->Start()) {
    return false;
  }
  return true;
}

void StorageService::Stop() {
  // Meta data manager must stop first,
  // Since it call buffer manager to flush its data
  meta_data_manager_->Stop();
  buffer_manager_->Stop();
  BOOST_LOG_TRIVIAL(info) << "storage service stop.";
}

bool StorageService::CreateRelation(const TableSchema & schema) {
  PageID id;
  bool ok = space_manager_->CreateSegment(&id);
  if (ok) {
    Relation* rel = new Relation();
    rel->SetName(schema.name_);
    rel->SetID(id.pageno_);
    std::vector<std::pair<ColumnSchema, int> > columns;
    for (int i = 0; i < schema.column_list_.size(); i++) {
      columns.push_back(std::make_pair(schema.column_list_[i], i));
    }

    /*std::sort(
        columns.begin(), columns.end(),
        [](const std::pair<ColumnSchema, int> & x, const std::pair<ColumnSchema, int> & y)
        { return x.first.data_type_ < y.first.data_type_;});
    */
    for (size_t i = 0; i < columns.size(); i++) {
      ColumnSchema & col = columns[i].first;
      Attribute attribute;
      attribute.SetName(col.name_);
      attribute.SetID(columns[i].second);
      attribute.SetDataType(col.data_type_);
      attribute.SetNull(col.is_null_);
      uint32_t size = 0;
      bool variable = false;
      switch (col.data_type_) {
        case kDTInteger:
          size = sizeof(int64_t);
          break;
        case kDTFloat:
          size = sizeof(int64_t);
          break;
        case kDTVarchar:
          variable = true;
          size = col.length_;
          break;
        case kDTDate:
          size = sizeof(int64_t);
          break;
        default:
          assert(false);
          break;
      }
      attribute.SetMaxLength(size);
      attribute.SetVariable(variable);

      rel->AddAttribute(attribute);
    }
    meta_data_manager_->AddRelation(rel);
  }

  return ok;
}

bool StorageService::DropRelation(const std::string & rel_name) {
  Relation *rel = meta_data_manager_->GetRelationByName(rel_name);
  if (rel == NULL) {
    return false;
  }
  // TODO ..

  meta_data_manager_->RemoveRelationByID(rel->GetID());

  return true;
}

IteratorInterface * StorageService::NewIterator(const std::string & rel_name) {
  Relation *rel = meta_data_manager_->GetRelationByName(rel_name);
  if (rel == NULL) {
    return NULL;
  }
  return new Iterator(rel->GetID(),  buffer_manager_ );
}

WriteBatchInterface * StorageService::NewWriteBatch(
    const std::string & rel_name) {
  Relation *rel = meta_data_manager_->GetRelationByName(rel_name);
  if (rel == NULL) {
    return NULL;
  }
  return new WriteBatch(rel->GetID(), space_manager_);
}

void StorageService::DeleteIOObject(IOObjectInterface* io_object) {
  delete io_object;
}

void StorageService::InitDB() {
  space_manager_->InitDB();
}

void StorageService::FlushAll() {
  buffer_manager_->FlushAll();
}


StorageServiceInterface* StorageServiceInterface::Instance()
{
  return & Storage::instance();
}

int import_from_csv(const std::string table, const std::string path) {
  storage::Loader *loader = new storage::Loader(path, table);
  bool ok = loader->Load();
  if (!ok) {
    BOOST_LOG_TRIVIAL(info) << "load failed ...";
    delete loader;
    return -1;
  }
  delete loader;
  BOOST_LOG_TRIVIAL(info) << "load finish ...";
  return 0;
}

bool StorageServiceInterface::Load(const std::string & table_name, const std::string & file_path)
{
  return import_from_csv(table_name, file_path) == 0;
}

}  // namespace storage
