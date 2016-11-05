#include "parser/select_stmt.h"
#include <sstream>
using std::stringstream;

ptree SelectTarget::ToPropertyTree() const {
  ptree select_target;
  if (expression_) {
    select_target.put_child(STR_SELECT_EXPRESSION,
                            expression_->ToPropertyTree());
  }
  if (!alias_name_.empty())
    select_target.put(STR_ALIAS_NAME, alias_name_);
  return select_target;
}

ptree OrderClause::ToPropertyTree() const {
  ptree order_clause;
  if (expr_) {
    order_clause.put_child(STR_SELECT_EXPRESSION, expr_->ToPropertyTree());
  }
  order_clause.put(STR_IS_DESC, desc_);
  return order_clause;
}

ptree SelectStmt::ToPropertyTree() const {
  ptree select_stmt;

  if (!select_list_.empty()) {
    // SELECT col1, col2, col3 ...
    ptree select_list;
    for (size_t i = 0; i < select_list_.size(); i++) {
      std::stringstream ssm;
      ssm << i;
      select_list.push_back(
          std::make_pair(ssm.str(), select_list_[i]->ToPropertyTree()));
    }
    select_stmt.put_child(STR_SELECT_LIST, select_list);
  }

  if (!table_reference_.empty()) {
    // FROM T1, (SELECT * FROM T2) ...
    ptree table_factor_list;
    for (size_t i = 0; i < table_reference_.size(); i++) {
      std::stringstream ssm;
      ssm << i;
      table_factor_list.push_back(
          std::make_pair(ssm.str(), table_reference_[i]->ToPropertyTree()));
    }
    select_stmt.put_child(STR_TABLE_FACTOR_LIST, table_factor_list);
  }

  if (!opt_where_.empty()) {
    // WHERE ...
    ptree where_list;
    for (size_t i = 0; i < opt_where_.size(); i++) {
      std::stringstream ssm;
      ssm << i;
      where_list.push_back(
          std::make_pair(ssm.str(), opt_where_[i]->ToPropertyTree()));
    }
    select_stmt.put_child(STR_WHERE_LIST, where_list);
  }

  if (!opt_group_.empty()) {
    // GROUP BY ...
    ptree groupby_list;
    for (size_t i = 0; i < opt_group_.size(); i++) {
      std::stringstream ssm;
      ssm << i;
      groupby_list.push_back(
          std::make_pair(ssm.str(), opt_group_[i]->ToPropertyTree()));
    }
    select_stmt.put_child(STR_GROUP_BY_LIST, groupby_list);
  }

  if (!opt_order_.empty()) {
    // ORDER BY ...
    ptree orderby_list;
    for (size_t i = 0; i < opt_order_.size(); i++) {
      std::stringstream ssm;
      ssm << i;
      orderby_list.push_back(
          std::make_pair(ssm.str(), opt_order_[i]->ToPropertyTree()));
    }
    select_stmt.put_child(STR_ORDER_BY_LIST, orderby_list);
  }
  return select_stmt;
}
