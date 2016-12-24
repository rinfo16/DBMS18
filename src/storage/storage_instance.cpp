#include "storage/storage_service_interface.h"
#include "level_store.h"
#include "common/singleton.hpp"

// Modify this to change another storage engine

namespace storage {
typedef utils::Singleton<storage::LevelStore> Storage;
StorageServiceInterface* StorageServiceInterface::Instance() {

  return &Storage::instance();
}
} // end namespace storage

