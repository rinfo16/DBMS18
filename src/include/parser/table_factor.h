#ifndef __TABLE_FACTOR_H__
#define __TABLE_FACTOR_H__

#include <string>

#include "parser/ast_base.h"
namespace ast {
class TableFactor : public ASTBase {
  friend ParserContext;
 public:
  TableFactor(TreeType type)
      : ASTBase(type) {
  }

  virtual ptree ToPropertyTree() const { return ptree(); }
};
}
#endif // __TABLE_FACTOR_H__
