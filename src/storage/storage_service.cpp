#include "storage_service.h"

namespace storage {

StorageService::StorageService() {

}

StorageService::~StorageService() {
}

RelationHandlerInterface * StorageService::OpenHandler(relationid_t rel, int mode) {
  return NULL;
}

void StorageService::CloseHandler(RelationHandlerInterface* handler) {
}
} // namespace storage
