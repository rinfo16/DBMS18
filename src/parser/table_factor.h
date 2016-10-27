#ifndef __TABLE_FACTOR_H__
#define __TABLE_FACTOR_H__

#include <string>

#include "parser/ast_base.h"

using std::string;

class TableFactor : public ASTBase {
 public:
  TableFactor()
      : sub_query_(NULL) {
  }

  string table_name_;
  string alias_name_;
  ASTBase *sub_query_;

  TRACE_FUNC_DECLARE;
};

#endif // __TABLE_FACTOR_H__
