#ifndef SRC_INCLUDE_STORAGE_LOADER_H_
#define SRC_INCLUDE_STORAGE_LOADER_H_

#include "common/minicsv.h"
#include "common/tuple.h"
#include "write_handler.h"

namespace storage {

class Loader {
 public:
  Loader(const std::string & path, const std::string & rel_name, int delimiter);
  bool Prepare();
  bool Load();
  virtual ~Loader();
 private:
  WriteBatchInterface *batch_;
  mini::csv::ifstream is_;
  std::string rel_name_;
  std::string csv_;
  int delimiter_;
  Tuple tuple_;
};

}  // namespace storage

#endif // SRC_INCLUDE_STORAGE_LOADER_H_
