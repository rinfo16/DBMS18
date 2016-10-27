#ifndef __JOIN_TABLE_H__
#define __JOIN_TABLE_H__

#include "parser/ast_base.h"

class JoinTable : public ASTBase {
 public:
  JoinTable()
      : left_table_(NULL),
        right_table_(NULL),
        join_predicate_(NULL),
        join_type_ (0){
  }

  ASTBase *left_table_;
  ASTBase *right_table_;
  ASTBase *join_predicate_;
  join_type_t join_type_;

  TRACE_FUNC_DECLARE;
};

#endif // __JOIN_TABLE_H__
