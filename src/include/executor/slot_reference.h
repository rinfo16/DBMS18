#ifndef SLOT_REFERENCE_EXPR_H__
#define SLOT_REFERENCE_EXPR_H__

#include "executor/value_expr_interface.h"
#include "common/tuple_row.h"
#include "common/define.h"

class SlotReference : public ValueExprInterface {
 public:
  SlotReference(uint32_t tuple_index, uint32_t slot_index);
  virtual const char *GetValue(TupleRow *row, uint32_t *length);
 private:
  uint32_t tuple_index_;
  uint32_t slot_index_;
};

#endif // SLOT_REFERENCE_EXPR_H__
