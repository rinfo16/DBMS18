#ifndef __SELECT_STMT_H__
#define __SELECT_STMT_H__

#include <vector>
#include <string>

#include "parser/ast_base.h"

using std::vector;
using std::string;

class OrderBy : public ASTBase {
 public:
  OrderBy()
      : expr_(NULL),
        order_(ORD_ASC) {
  }

  ASTBase * expr_;
  order_t order_;

  TRACE_FUNC_DECLARE;
};

class SelectStmt : public ASTBase {
 public:
  SelectStmt()
      : select_list_(NULL),
        table_reference_(NULL),
        opt_where_(NULL),
        opt_group_(NULL),
        opt_have_(NULL),
        opt_order_(NULL) {
  }
  ;

  ~SelectStmt() {
  }
  ;

  ASTBase *select_list_;
  ASTBase *table_reference_;
  ASTBase *opt_where_;
  ASTBase *opt_group_;
  ASTBase *opt_have_;
  ASTBase *opt_order_;

  TRACE_FUNC_DECLARE;
};

#endif // __SELECT_STMT_H__
