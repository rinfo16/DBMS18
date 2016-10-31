#ifndef EXPRESSION_H__
#define EXPRESSION_H__


#include <string>
#include "parser/ast_base.h"



class Expression : public ASTBase {
 public:
  Operator operator_;
  ASTBase *left_;
  ASTBase *right_;

  TRACE_FUNC_DECLARE;
};



#endif // EXPRESSION_H__
