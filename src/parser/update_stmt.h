#include "parser/ast_base.h"
#include <string>

class UpdateStmt : public ASTBase {
 public:

  UpdateStmt()
      : table_name_(NULL),
        set_clause_list_(NULL),
        opt_from_clause_(NULL),
        opt_where_(NULL) {
  }

  ASTBase *table_name_;
  ASTBase * set_clause_list_;
  ASTBase * opt_from_clause_;
  ASTBase * opt_where_;

  TRACE_FUNC_DECLARE;
};
