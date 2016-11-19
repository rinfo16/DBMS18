#ifndef __JOIN_TABLE_H__
#define __JOIN_TABLE_H__

#include "parser/ast_base.h"
#include "parser/table_factor.h"
#include "parser/expression_base.h"

namespace ast {
class JoinClause : public TableFactor {
  friend ParserContext;
 public:
  JoinClause(TableFactor *left, TableFactor *right, ExpressionBase *condition,
             JoinType type)
      : TableFactor(kASTJoinClause),
        left_table_(left),
        right_table_(right),
        join_predicate_(condition),
        join_type_(type) {
  }

  const TableFactor *LeftTable() const {
    return left_table_;
  }

  const TableFactor *RightTable() const {
    return right_table_;
  }

  const ExpressionBase *JoinPredicate() const {
    return join_predicate_;
  }
  JoinType Type() const {
    return join_type_;
  }
  virtual ptree ToPropertyTree() const;
 private:
  JoinClause();
  JoinType join_type_;
  TableFactor *left_table_;
  TableFactor *right_table_;
  ExpressionBase *join_predicate_;
};
}
#endif // __JOIN_TABLE_H__
