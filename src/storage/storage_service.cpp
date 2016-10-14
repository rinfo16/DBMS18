#include "storage_service.h"
#include "common/config.h"
#include "relation_handler.h"
namespace storage {

StorageService::StorageService() {
  buffer_manager_ = new BufferManager(
      config::Setting::instance().buffer_pool_size_,
      config::Setting::instance().page_size_,
      config::Setting::instance().data_directory_);
  space_manager_ = new SpaceManager(buffer_manager_);
}

StorageService::~StorageService() {
  delete space_manager_;
  delete buffer_manager_;
}
bool StorageService::Start() {
  return buffer_manager_->Start();
}

RelationHandlerInterface * StorageService::OpenHandler(relationid_t rel,
                                                       OpenMode mode) {
  RelationHandler *handler = new RelationHandler(rel, mode, buffer_manager_,
                                                 space_manager_);
  return handler;
}

void StorageService::CloseHandler(RelationHandlerInterface* handler) {
  delete handler;
}

void StorageService::InitDB() {
  SpaceManager::InitDB();
}

void StorageService::FlushAll() {
  buffer_manager_->FlushAll();
}
}  // namespace storage
