#include "executor/project.h"

namespace executor {

Project::Project(ExecInterface *left,
                 const std::vector<ValueExprInterface*> & project_attr,
                 int32_t tuple_count)
    : left_(left),
      project_attr_(project_attr) {
  tuple_row_ = new TupleRow(tuple_count);
}

Project::~Project() {
  delete tuple_row_;
}

State Project::Prepare() {
  return left_->Prepare();
}

State Project::Open() {
  return left_->Open();
}

State Project::GetNext(TupleRow *row) {
  State state = left_->GetNext(tuple_row_);
  if (state != kStateOK) {
    return state;
  }
  uint32_t tuple_size = sizeof(Slot) * project_attr_.size();
  for (int i = 0; i < project_attr_.size(); i++) {
    ValueExprInterface *value_expr = project_attr_[i];
    uint32_t size = 0;
    value_expr->GetValue(tuple_row_, &size);
    tuple_size += size;
  }

  Tuple tuple = memory::CreateTuple(tuple_size);
  uint32_t off = sizeof(Slot) * project_attr_.size();
  for (int i = 0; i < project_attr_.size(); i++) {
    ValueExprInterface *value_expr = project_attr_[i];
    uint32_t size = 0;
    const char *value = value_expr->GetValue(tuple_row_, &size);
    Slot *slot = tuple->GetSlot(i);
    slot->length_ = size;
    slot->offset_ = off;
    tuple->SetValue(off, value, size);
    off += size;
  }

  row->SetTuple(0, tuple);

  return kStateOK;
}

 void Project::Close() {
  left_->Close();
}

}  // end namespace executor
