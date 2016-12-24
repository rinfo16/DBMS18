#ifndef SRC_INCLUDE_STORAGE_LOADER_H_
#define SRC_INCLUDE_STORAGE_LOADER_H_

#include "common/minicsv.h"
#include "common/tuple.h"
#include "common/relation.h"
#include "storage/write_handler.h"
#include "executor/exec_interface.h"
#include "storage/storage_service_interface.h"

namespace executor {

class Load : public CmdInterface {
 public:
  Load(const std::string & path, const std::string & rel_name);
  virtual State Prepare();
  virtual State Exec();
  virtual ~Load();
 private:
  storage::StorageServiceInterface *store_;
  storage::WriteHandler *batch_;
  mini::csv::ifstream is_;
  std::string rel_name_;
  std::string csv_;
  Tuple tuple_;
};

}  // namespace storage

#endif // SRC_INCLUDE_STORAGE_LOADER_H_
