#include "parser/create_stmt.h"

ptree ColumnDefine::ToPropertyTree() const {
  ptree colunn_define;
  colunn_define.put(STR_COLUMN_NAME, column_name_);
  colunn_define.put(STR_DATA_TYPE, data_type_);
  colunn_define.put(STR_DATA_TYPE_LENGTH, data_length_);
  return colunn_define;
}

ptree CreateStmt::ToPropertyTree() const {
  ptree create;
  create.put(STR_TABLE_NAME, table_name_);

  ptree column_name_list;
  for (size_t i = 0; i < column_define_list_.size(); i++) {
    std::stringstream ssm;
    ssm << i;
    column_name_list.push_back(
        std::make_pair(ssm.str(), column_define_list_[i]->ToPropertyTree()));
  }
  create.put_child(STR_COLUMN_DEFINE_LIST, column_name_list);

  return create;
}
