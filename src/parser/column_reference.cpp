#include "parser/column_reference.h"
namespace ast {
ptree ColumnReference::ToPropertyTree() const {
  ptree column_reference;
  if (!table_name_.empty()) {
    column_reference.put(STR_TABLE_NAME, table_name_);
  }
  if (!column_name_.empty()) {
    column_reference.put(STR_COLUMN_NAME, column_name_);
  }
  if (!alias_name_.empty()) {
    column_reference.put(STR_ALIAS_NAME, alias_name_);
  }
  return column_reference;
}
}
