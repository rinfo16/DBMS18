#include "parser_context.h"
#include "name.h"
#include "value.h"
#include "expression.h"
#include "create_stmt.h"
#include "update_stmt.h"
#include "insert_stmt.h"
#include "join_table.h"
#include "select_stmt.h"
#include "table_factor.h"

ParserContext::~ParserContext() {
  size_t size = ast_node_list_.size();
  for (size_t i = 0; i < size; i++) {
    delete ast_node_list_[i];
  }
}

ASTBase *ParserContext::new_create_table_stmt(const char * tbl_name,
                                              table_scope_t scope,
                                              ASTBase * col_def) {
  CreateStmt *stmt = new CreateStmt;
  ast_node_list_.push_back(stmt);
  stmt->table_name_ = tbl_name;
  stmt->table_scope_ = scope;
  stmt->column_define_list_ = col_def;
  return stmt;
}

ASTBase *ParserContext::new_create_table_as_stmt(const char * tbl_name,
                                                 table_scope_t scope,
                                                 ASTBase * col_names,
                                                 ASTBase *subquery) {
  CreateStmt *stmt = new CreateStmt;
  ast_node_list_.push_back(stmt);
  stmt->table_name_ = tbl_name;
  stmt->table_scope_ = scope;
  stmt->opt_column_name_list_ = col_names;
  stmt->subquery_ = subquery;
  return stmt;
}

ASTBase *ParserContext::new_insert_stmt(const char* tbl_name,
                                        ASTBase *col_names, ASTBase *vals) {
  InsertStmt *stmt = new InsertStmt;
  ast_node_list_.push_back(stmt);
  stmt->table_name_ = tbl_name;
  stmt->opt_column_names_ = col_names;
  stmt->insert_values_ = vals;
  return stmt;
}

ASTBase * ParserContext::new_select_stmt(ASTBase* sel_list,
                                         ASTBase* tbl_ref_list,
                                         ASTBase* opt_where, ASTBase* opt_group,
                                         ASTBase* opt_have,
                                         ASTBase* opt_order) {
  SelectStmt *stmt = new SelectStmt;
  ast_node_list_.push_back(stmt);

  stmt->select_list_ = sel_list;
  stmt->table_reference_ = tbl_ref_list;
  stmt->opt_where_ = opt_where;
  stmt->opt_group_ = opt_group;
  stmt->opt_have_ = opt_have;
  stmt->opt_order_ = opt_order;

  return stmt;
}

ASTBase *ParserContext::new_name(const char *str) {
  Name *nm = new Name;
  ast_node_list_.push_back(nm);

  nm->str_val_ = str;

  return nm;
}

ASTBase *ParserContext::new_value(const char *str) {
  Value *nm = new Value;
  ast_node_list_.push_back(nm);

  nm->str_val_ = str;

  return nm;
}

ASTBase * ParserContext::new_orderby(ASTBase *expr, order_t ord) {
  OrderBy *orderby = new OrderBy;
  ast_node_list_.push_back(orderby);

  orderby->expr_ = expr;
  orderby->order_ = ord;

  return orderby;
}

ASTBase * ParserContext::new_table_factor(const char* tbl, const char* alias,
                                          ASTBase *sub) {
  TableFactor *tbl_factor = new TableFactor;
  ast_node_list_.push_back(tbl_factor);

  tbl_factor->table_name_ = (tbl == NULL ? "" : tbl);
  tbl_factor->alias_name_ = (alias == NULL ? "" : alias);
  tbl_factor->sub_query_ = sub;

  return tbl_factor;
}

ASTBase * ParserContext::new_join_table(ASTBase *left, ASTBase *right,
                                        ASTBase *cond, join_type_t type) {
  JoinTable *join_tbl = new JoinTable;
  ast_node_list_.push_back(join_tbl);

  join_tbl->left_table_ = left;
  join_tbl->right_table_ = right;
  join_tbl->join_predicate_ = cond;
  join_tbl->join_type_ = type;

  return join_tbl;
}

ASTBase *ParserContext::new_column_def(const char *col_name, data_type_t type) {
  ColumnDef *col_def = new ColumnDef;
  ast_node_list_.push_back(col_def);

  col_def->column_name_ = col_name;
  col_def->data_type_ = type;

  return col_def;
}

ASTBase *ParserContext::new_expression(Operator op, ASTBase *left,
                                       ASTBase *right) {
  Expression *expression = new Expression;
  ast_node_list_.push_back(expression);
  expression->left_ = left;
  expression->right_ = right;
  expression->operator_ = op;
  return expression;
}

ASTBase *ParserContext::NewUpdateStmt(ASTBase *table_name,
                                      ASTBase * set_clause_list,
                                      ASTBase * opt_from_clause,
                                      ASTBase * opt_where) {
  UpdateStmt *update_stmt = new UpdateStmt();
  update_stmt->table_name_ = table_name;
  update_stmt->set_clause_list_ = set_clause_list;
  update_stmt->opt_from_clause_ = opt_from_clause;
  update_stmt->opt_where_ = opt_where;
  return update_stmt;
}
