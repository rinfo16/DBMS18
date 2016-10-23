#include "storage_service.h"
#include "relation_handler.h"
#include "write_batch.h"
#include "iterator.h"

namespace storage {

StorageService::StorageService() {
  buffer_manager_ = new BufferManager();
  space_manager_ = new SpaceManager(buffer_manager_);
}

StorageService::~StorageService() {
  delete space_manager_;
  delete buffer_manager_;
}
bool StorageService::Start() {
  return buffer_manager_->Start();
}

bool StorageService::CreateRelation(relationid_t rel) {
  RelationHandler *handler = new RelationHandler(rel, kRelationCreate,
                                                 buffer_manager_,
                                                 space_manager_);
  bool ok = handler->Create();
  delete handler;
  return ok;
}

bool StorageService::DropRelation(relationid_t rel) {
  RelationHandler *handler = new RelationHandler(rel, kRelationDrop,
                                                 buffer_manager_,
                                                 space_manager_);
  bool ok = handler->Drop();
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
