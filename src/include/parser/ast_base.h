#ifndef __AST_BASE_H__
#define __AST_BASE_H__

#include <vector>
#include <iostream>
#include <cstddef>
#include "ast_def.h"
#include "trace_ast.h"

class ASTBase {
 public:
  ASTBase()
      : prev_(NULL),
        next_(NULL) {
  }
  ;

  void rappend(ASTBase *ast) {
    ASTBase *last = this;
    while (last->next_) {
      last = last->next_;
    }
    ast->prev_ = last;
    last->next_ = ast;
  }

  void lappend(ASTBase *ast) {
    ASTBase *first = this;
    while (first->prev_) {
      first = first->prev_;
    }
    ast->next_ = first;
    first->prev_ = ast;
  }

  virtual ~ASTBase() {
  }

  ASTBase *prev_;
  ASTBase *next_;

  TRACE_FUNC_DECLARE;
};

#endif
