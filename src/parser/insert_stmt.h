
#ifndef __INSERT_STMT_H__
#define __INSERT_STMT_H__

#include <string>
#include "parser/ast_base.h"

class InsertStmt : public ASTBase {
 public:
  InsertStmt()
      : opt_column_names_(NULL),
        insert_values_(NULL) {
  }

  string table_name_;
  ASTBase* opt_column_names_;
  ASTBase* insert_values_;

  TRACE_FUNC_DECLARE;
};

#endif //__INSERT_STMT_H__
