#include "executor/insert_value.h"

namespace executor {

InsertValue::InsertValue(storage::WriteHandler *handler, Tuple tuple,
                         size_t size) {
  handler_ = handler;
  tuple_ = tuple;
  size_ = size;
}

State InsertValue::Prepare() {
  return kStateOK;
}
State InsertValue::Exec() {
  TupleWarpper t(tuple_.get(), size_);
  handler_->Put(&t);
  if (handler_->Commit()) {
    SetResponse("insert 1 row");
    return kStateOK;
  } else {
    SetResponse("insert failed");
    return kStateInsertFailed;
  }
}
}
