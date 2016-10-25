#include "storage_service.h"
#include "relation_handler.h"
#include "write_batch.h"
#include "iterator.h"
#include "meta_data_manager.h"


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
  return meta_data_manager_->Start();
}

void StorageService::Stop() {
  // Meta data manager must stop first,
  // Since it call buffer manager to flush its data
  meta_data_manager_->Stop();
  buffer_manager_->Stop();
}

bool StorageService::CreateRelation(const TableSchema & schema) {
  PageID id;
  bool ok = space_manager_->CreateSegment(&id);
  if (ok) {
    Relation* rel = new Relation();
    rel->SetName(schema.name_);
    rel->SetID(id.pageno_);
    std::vector<std::pair<Column, int> > columns;
    for (int i = 0; i < schema.column_list_.size(); i++) {
      columns.push_back(std::make_pair(schema.column_list_[i], i));
    }

    std::sort(
        columns.begin(), columns.end(),
        [](const std::pair<Column, int> & x, const std::pair<Column, int> & y)
        { return x.first.data_type_ < y.first.data_type_;});

    for (size_t i = 0; i < columns.size(); i++) {
      Column & col = columns[i].first;
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
  RelationHandler *handler = new RelationHandler(rel->GetID(), kRelationDrop,
                                                 buffer_manager_,
                                                 space_manager_);
  bool ok = handler->Drop();
  if (ok) {
    meta_data_manager_->RemoveRelationByID(rel->GetID());
  }
  delete handler;
  return ok;
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

}  // namespace storage
