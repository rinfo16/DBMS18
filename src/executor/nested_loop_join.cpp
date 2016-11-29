#include "executor/nested_loop_join.h"

namespace executor {

NestedLoopJoin::NestedLoopJoin(int32_t tuple_count,
                               ExecInterface *left, ExecInterface *right,
                               const BooleanExprInterface *join_predicate)
    : left_(left),
      right_(right),
      join_predicate_(join_predicate),
      row_(NULL),
      left_state_(kStateOK),
      right_state_(kStateOK){
  row_ = new TupleRow(tuple_count);
}

NestedLoopJoin::~NestedLoopJoin() {
  delete row_;
}

State NestedLoopJoin::Prepare() {
  State state = left_->Prepare();
  if (state != kStateOK) {
    return state;
  }
  state = right_->Prepare();
  if (state != kStateOK) {
    return state;
  }
  return kStateOK;
}

State NestedLoopJoin::Open() {
  State state = left_->Open();
  if (state != kStateOK) {
    return state;
  }
  state = right_->Open();
  if (state != kStateOK) {
    return state;
  }
  // get the first row of left table
  left_state_ = left_->GetNext(row_);
  if (left_state_ != kStateEOF && left_state_ != kStateOK) {
    return left_state_;
  }
  right_state_ = kStateOK;

  return kStateOK;
}

State NestedLoopJoin::GetNext(TupleRow *row) {
  while (true) {
    if (left_state_ == kStateEOF && right_state_ == kStateEOF)
      return kStateEOF;

    right_state_ = right_->GetNext(row_);
    if (right_state_ == kStateOK) {
      if (join_predicate_->GetValue(row_)) {
        // output a row
        row_->CopyTo(row);
        return kStateOK;
      }
    }
    else if (right_state_ == kStateEOF){
      // right table reach EOF, then read the next row of left table
      left_state_ = left_->GetNext(row_);
      if (left_state_ != kStateOK) {
        // return EOF or an error ..
        return left_state_;
      }

      // seek to begin of the right table and continue ...
      right_state_ = right_->Open();
      if (right_state_ != kStateOK) {
        // return EOF or an error ..
        return right_state_;
      }
    }
    else {
      // right GetNext return an error
      return right_state_;
    }
  }
  return kStateOK;
}

void NestedLoopJoin::Close() {
  left_->Close();
  right_->Close();
}

}  // namespace executor
