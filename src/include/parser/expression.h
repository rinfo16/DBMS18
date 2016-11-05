#ifndef EXPRESSION_H__
#define EXPRESSION_H__

#include <string>
#include "parser/ast_base.h"
#include "parser/expression_base.h"

class Expression : public ExpressionBase {
 public:
  Expression(ExpressionBase *left, ExpressionBase*right, OperatorType op)
      : left_(left),
        right_(right),
        operator_(op),
        ExpressionBase(kASTExpression) {
  }

  OperatorType ExprOperator() const {
    return operator_;
  }
  ExpressionBase *Left() {
    return left_;
  }
  ExpressionBase *Right() {
    return right_;
  }
  const ExpressionBase *Left() const {
    return left_;
  }
  const ExpressionBase *Right() const {
    return right_;
  }

  virtual ptree ToPropertyTree() const;
 private:
  Expression();
  OperatorType operator_;
  ExpressionBase *left_;
  ExpressionBase *right_;
};

#endif // EXPRESSION_H__
