#ifndef SLOT_REFERENCE_EXPR_H__
#define SLOT_REFERENCE_EXPR_H__

#include "executor/value_expr_interface.h"
#include "common/tuple_row.h"
#include "common/define.h"
namespace executor {
class SlotReference : public ValueExprInterface {
 public:
  SlotReference();
  SlotReference(int32_t tuple_index, int32_t slot_index);
  virtual const char *GetValue(TupleRow *row, uint32_t *length) const;
  void SetTupleIndex(int32_t tuple_index) { tuple_index_ = tuple_index ; }
  void SetSlotIndex(int32_t slot_index) { slot_index_ = slot_index ; }
 private:
  int32_t tuple_index_;
  int32_t slot_index_;
};
}
#endif // SLOT_REFERENCE_EXPR_H__
