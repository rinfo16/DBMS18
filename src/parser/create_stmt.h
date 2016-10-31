#ifndef __CREATE_TABLE_STMT_H__
#define __CREATE_TABLE_STMT_H__

#include <string>
#include "parser/ast_base.h"

using std::string;

class CreateStmt : public ASTBase {
 public:

  CreateStmt()
      : column_define_list_(NULL),
        opt_column_name_list_(NULL),
        subquery_(NULL),
        table_scope_(0) {
  }

  string table_name_;
  ASTBase * column_define_list_;
  ASTBase * opt_column_name_list_;
  ASTBase * subquery_;
  table_scope_t table_scope_;

  TRACE_FUNC_DECLARE;
};

class ColumnDef : public ASTBase {
 public:
  string column_name_;
  data_type_t data_type_;

  TRACE_FUNC_DECLARE;
};

#endif // __CREATE_TABLE_STMT_H__
