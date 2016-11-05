#include "parser/const_value.h"

ptree ConstValue::ToPropertyTree() const {
  ptree value;
  if (!str_val_.empty())
    value.put(STR_CONST_VALUE, str_val_);
  else
    value.put(STR_CONST_VALUE, int_val_);
  return value;
}
