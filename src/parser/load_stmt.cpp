#include "parser/load_stmt.h"

ptree LoadStmt::ToPropertyTree() const {
  ptree load_stmt;
  if (!table_name_.empty()) {
    load_stmt.put(STR_TABLE_NAME, table_name_);
  }
  if (!opt_column_name_list_.empty()) {
    ptree column_name_list;
    for (size_t i = 0; i < opt_column_name_list_.size(); i++) {
      std::stringstream ssm;
      ssm << i;
      ptree column_name;
      column_name.put(STR_COLUMN_NAME, opt_column_name_list_[i]);
      column_name_list.push_back(std::make_pair(ssm.str(), column_name));
    }
    load_stmt.put_child(STR_COLUMN_NAME_LIST, column_name_list);
  }
  if (!file_path_.empty()) {
    load_stmt.put(STR_FILE_PATH, file_path_);
  }
  return load_stmt;
}
