#include "parser/table_reference.h"
namespace ast {
ptree TableReference::ToPropertyTree() const {
  ptree table_reference;
  if (!table_name_.empty()) {
    table_reference.put(STR_TABLE_NAME, table_name_);
  }
  if (!alias_name_.empty()) {
    table_reference.put(STR_ALIAS_NAME, alias_name_);
  }
  return table_reference;
}
}
