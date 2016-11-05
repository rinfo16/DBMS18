#ifndef EXPRESSION_BASE_H__
#define EXPRESSION_BASE_H__

#include "parser/ast_base.h"

class ExpressionBase : public ASTBase {
public:
  ExpressionBase(TreeType type): ASTBase(type) {}
  virtual ptree ToPropertyTree() const { return ptree(); }
private:
  ExpressionBase();
};

#endif //EXPRESSION_BASE_H__
