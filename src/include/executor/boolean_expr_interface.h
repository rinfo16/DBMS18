#ifndef BOOLEAN_EXPR_INTERFACE_H__
#define BOOLEAN_EXPR_INTERFACE_H__

#include "common/tuple_row.h"
#include "executor/datum_interface.h"

class BooleanExprInterface : public DatumInterface {
public:
  virtual ~BooleanExprInterface() {};
  virtual bool GetValue(TupleRow *row) = 0;
};

#endif // BOOLEAN_EXPR_INTERFACE_H__
