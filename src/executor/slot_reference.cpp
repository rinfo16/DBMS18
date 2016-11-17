#include "executor/slot_reference.h"

SlotReference::SlotReference(uint32_t tuple_index, uint32_t slot_index)
: tuple_index_(tuple_index),
slot_index_(slot_index)
{
}

const char *SlotReference::GetValue(TupleRow *row, uint32_t *length) {
  Tuple tuple = row->GetTuple(tuple_index_);
  const Slot *slot = tuple->GetSlot(slot_index_);
  *length = (uint32_t)slot->offset_;
  return (const char*) tuple->GetValue(slot->offset_);
}

