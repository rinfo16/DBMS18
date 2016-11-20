#include "executor/select.h"

namespace executor {

Select::Select(ExecInterface *left, const BooleanExprInterface *predicate)
    : left_(left),
      predicate_(predicate) {
}

Select::~Select() {
}

State Select::Prepare() {
  return left_->Prepare();
}

State Select::Open() {
  return left_->Open();
}

State Select::GetNext(TupleRow *row) {
  return left_->GetNext(row);
}

void Select::Close() {
  left_->Close();
}

}  // namespace executor
