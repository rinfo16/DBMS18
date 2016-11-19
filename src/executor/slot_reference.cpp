#include "executor/slot_reference.h"
namespace executor {
SlotReference::SlotReference()
    : tuple_index_(-1),
      slot_index_(-1) {
}

SlotReference::SlotReference(int32_t tuple_index, int32_t slot_index)
    : tuple_index_(tuple_index),
      slot_index_(slot_index) {
}

const char *SlotReference::GetValue(TupleRow *row, uint32_t *length) const {
  assert(tuple_index_ >= 0 && slot_index_ >= 0);
  Tuple tuple = row->GetTuple(tuple_index_);
  const Slot *slot = tuple->GetSlot(slot_index_);
  *length = (uint32_t) slot->offset_;
  return (const char*) tuple->GetValue(slot->offset_);
}

}
