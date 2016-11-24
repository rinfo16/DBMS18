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
  State state = left_ -> GetNext(row);
  while (true) {
      if (state == kStateEOF )
        return kStateEOF;
      else
        return left_->GetNext(row);
      state = left_->GetNext(row);
    }
}

void Select::Close() {
  left_->Close();
}

}  // namespace executor
