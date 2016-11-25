#ifndef EXPORT_H__
#define EXPORT_H__

#include "executor/exec_interface.h"
#include "common/minicsv.h"
#include "common/row_desc.h"

namespace executor {
class Export : public CmdInterface {
 public:
  Export(ExecInterface *tupler, const RowDesc *desc,
         const std::string & file_path);
  ~Export();
  virtual State Prepare();
  virtual State Exec();
 private:
  const RowDesc *desc_;
  ExecInterface *tupler_;
  mini::csv::ofstream os_;
  std::string relation_name_;
  std::string file_path_;
  Tuple tuple_;
  int32_t rows_;
};
}  // namespace executor

#endif
