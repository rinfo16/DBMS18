#include "executor/select.h"

namespace executor {

Select::Select(ExecInterface *left, const BooleanExprInterface *predicate)//predicate shiyigeguolvqi,yejiushi where tiaojian
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
  while (true) {
      State state = left_ -> GetNext(row);
      if (state == kStateEOF )
        return kStateEOF;
      else
      {
        bool pre = predicate_->GetValue(row);
        if(pre)
          return kStateOK;
      }
    }
}

void Select::Close() {
  left_->Close();
}

}  // namespace executor
