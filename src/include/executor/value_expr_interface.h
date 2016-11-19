#ifndef VALUE_EXPR_INTERFACE_H__
#define VALUE_EXPR_INTERFACE_H__

#include "executor/datum_interface.h"
#include "common/tuple_row.h"
namespace executor {
class ValueExprInterface : public DatumInterface {
public:
  virtual ~ValueExprInterface() {}
  virtual const char *GetValue(TupleRow *row, uint32_t *length) const = 0;
};
}
#endif // VALUE_EXPR_INTERFACE_H__
