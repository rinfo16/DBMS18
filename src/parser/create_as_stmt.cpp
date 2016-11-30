#include "parser/create_as_stmt.h"
#include <vector>
#include <string>
#include <sstream>
#include <utility>

using std::vector;
using std::string;
using std::pair;

namespace ast {

ptree CreateAsStmt::ToPropertyTree() const {
  ptree create_as;
  create_as.put(STR_TABLE_NAME, table_name_);

  if (!opt_column_name_list_.empty()) {
    ptree column_name_list;
    for (size_t i = 0; i < opt_column_name_list_.size(); i++) {
      std::stringstream ssm;
      ssm << i;
      ptree column_name;
      column_name.put(STR_COLUMN_NAME, opt_column_name_list_[i]);
      column_name_list.push_back(
          std::make_pair(ssm.str(), column_name));
    }
    create_as.put_child(STR_COLUMN_NAME_LIST, column_name_list);
  }
  if (sub_select_) {
    create_as.put_child(STR_SUB_SELECT, sub_select_->ToPropertyTree());
  }

  return create_as;
}
}
