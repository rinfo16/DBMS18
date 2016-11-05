#ifndef __SELECT_STMT_H__
#define __SELECT_STMT_H__

#include <vector>
#include <string>

#include "parser/ast_base.h"
#include "parser/expression.h"
#include "parser/table_factor.h"
#include "parser/expression_base.h"

using std::vector;
using std::string;


class SelectTarget : public ASTBase {
 public:
  SelectTarget(ExpressionBase *expr)
      : expression_(expr),
        ASTBase(kASTSelectTarget) {

  }

  void SetAliasName(const std::string & name) {
    alias_name_ = name;
  }

  const ExpressionBase *SelectExpression() const {
    return expression_;
  }

  const std::string & AliasName() const {
    return alias_name_;
  }

  virtual ptree ToPropertyTree() const;
 private:
  SelectTarget();
  ExpressionBase *expression_;
  std::string alias_name_;
};

class OrderClause : public ASTBase {
 public:
  OrderClause(ExpressionBase *ref, bool desc = true)
      : expr_(ref),
        desc_(desc),
        ASTBase(kASTOrderClause) {
  }

  ExpressionBase *OrderByExpression() {
    return expr_;
  }

  bool IsDesc() {
    return desc_;
  }
  virtual ptree ToPropertyTree() const;
 private:
  OrderClause();
  ExpressionBase * expr_;
  bool desc_;
};

class SelectStmt : public ASTBase {
  friend ParserContext;
 public:
  SelectStmt()
      : select_star_(false),
        ASTBase(kASTSelectStmt) {
  }
  ;

  ~SelectStmt() {
  }

  std::vector<SelectTarget*> & SelectList() {
    return select_list_;
  }

  const std::vector<SelectTarget*> & SelectList() const {
    return select_list_;
  }

  const std::vector<TableFactor*> & TableReference() const {
    return table_reference_;
  }

  const std::vector<ExpressionBase*> & OptWhere() const {
    return opt_where_;
  }

  const std::vector<ExpressionBase*> & OptGroupBy() const {
    return opt_group_;
  }

  const std::vector<OrderClause *> OptOrderBy() const {
    return opt_order_;
  }
  virtual ptree ToPropertyTree() const;
 private:
  bool select_star_;
  std::vector<SelectTarget*> select_list_;
  std::vector<TableFactor*> table_reference_;
  std::vector<ExpressionBase*> opt_where_;
  std::vector<ExpressionBase*> opt_group_;
  std::vector<OrderClause*> opt_order_;
};

#endif // __SELECT_STMT_H__
