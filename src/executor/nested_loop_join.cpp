#include "executor/nested_loop_join.h"

namespace executor {

NestedLoopJoin::NestedLoopJoin(ExecInterface *left, ExecInterface *right,
                               const BooleanExprInterface *join_predicate) {
}

NestedLoopJoin::~NestedLoopJoin() {
}

State NestedLoopJoin::Prepare() {
  return kStateOK;
}
State NestedLoopJoin::Open() {
  return kStateOK;
}
State NestedLoopJoin::GetNext(TupleRow *row) {
  return kStateOK;
}
void NestedLoopJoin::Close() {
}

}  // namespace executor
