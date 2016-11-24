#include "executor/export.h"
#include <iostream>
#include <sstream>

namespace executor {
Export::Export(ExecInterface *tupler, const RowDesc *row_desc,
               const std::string & file_path)
    : file_path_(file_path),
      desc_(row_desc),
      tupler_(tupler),
      rows_(0) {
}

Export::~Export() {
}

State Export::Prepare() {
  try {
    os_.open(file_path_.c_str());
    os_.set_delimiter(',', "$$");
    //os_.enable_surround_quote_on_str(true, '\"');
  } catch (...) {
    return kStateFileNotFind;
  }
  return kStateOK;
}

State Export::Exec() {
  TupleRow row(1);
  State state = tupler_->Open();
  while (state == kStateOK) {
    state = tupler_->GetNext(&row);
    if (state == kStateOK) {
      rows_++;
      auto columns = desc_->GetColumnCount();
      for (auto i = 0; i < columns; i++) {
        DataType type = desc_->GetColumnDesc(i).data_type_;
        const Tuple tuple = row.GetTuple(0);
        const Slot * slot = tuple->GetSlot(i);
        uint16_t off = slot->offset_;
        uint16_t len = slot->length_;
        switch (type) {
          case kDTInteger:
          case kDTDate: {
            int64_t v = tuple->GetInteger(off);
            os_ << v;
          }
            break;
          case kDTFloat: {
            double_t v = tuple->GetDouble(off);
            os_ << v;
          }
            break;
          case kDTVarchar: {
            std::string v((const char*) tuple->GetValue(off), len);
            os_ << v;
          }
            break;
          default:
            break;
        }
      }
      os_ << NEWLINE;
    }
  }
  tupler_->Close();
  os_.close();
  if (state == kStateEOF) {
    std::stringstream response;
    response << "copy " << rows_ << ".";
    SetResponse(response.str());
    return kStateOK;
  }
  else {
    SetResponse("copy failed.");
  }
  return state;
}

}  // namespace executor
