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
  return buffer_manager_->Start();
}

bool StorageService::CreateRelation(Relation* rel) {
  RelationHandler *handler = new RelationHandler(rel->GetID(), kRelationCreate,
                                                 buffer_manager_,
                                                 space_manager_);
  bool ok = handler->Create();
  if (ok) {
    meta_data_manager_->AddRelation(rel);
  }
  delete handler;
  return ok;
}

bool StorageService::DropRelation(relationid_t rel) {
  RelationHandler *handler = new RelationHandler(rel, kRelationDrop,
                                                 buffer_manager_,
                                                 space_manager_);
  bool ok = handler->Drop();
  if (ok) {
    meta_data_manager_->RemoveRelationByID(rel);
  }
  delete handler;
  return ok;
}

IteratorInterface * StorageService::NewIterator(relationid_t rel) {
  RelationHandler *handler = new RelationHandler(rel, kRelationRead,
                                                 buffer_manager_,
                                                 space_manager_);
  return new Iterator(handler);

}

WriteBatchInterface * StorageService::NewWriteBatch(relationid_t rel) {
  RelationHandler *handler = new RelationHandler(rel, kRelationWrite,
                                                 buffer_manager_,
                                                 space_manager_);
  return new WriteBatch(handler);
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

void StorageService::Stop() {
  buffer_manager_->Stop();
}

}  // namespace storage
