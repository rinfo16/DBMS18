#include <boost/algorithm/string.hpp>
#include "parser_context.h"

#include "parser/operation.h"
#include "parser/const_value.h"
#include "parser/join_clause.h"
#include "parser/reference_name.h"
#include "parser/table_reference.h"
#include "parser/create_stmt.h"
#include "parser/update_stmt.h"
#include "parser/insert_stmt.h"
#include "parser/select_stmt.h"
#include "parser/table_factor.h"
#include "parser/sub_query.h"
#include "parser/create_as_stmt.h"
#include "parser/column_reference.h"
#include "parser/load_stmt.h"
#include "parser/export_stmt.h"

using namespace ast;
ParserContext::~ParserContext() {
  size_t size = ast_node_list_.size();
  for (size_t i = 0; i < size; i++) {
    delete ast_node_list_[i];
  }
}

bool ParserContext::Parse() {
  return driver_.parse_string(*this, sql_, "INPUT");
}

ASTBase *ParserContext::GetAST() {
  return root_;
}

ASTBase *ParserContext::NewCreateStmt(ASTBase *table_name,
                                      ASTBase * col_def_list) {
  CreateStmt *stmt = new CreateStmt;
  ast_node_list_.push_back(stmt);
  stmt->table_name_ = dynamic_cast<ReferenceName*>(table_name)->String();
  ASTBase *ast = col_def_list;
  while (ast) {
    ColumnDefine *def = dynamic_cast<ColumnDefine*>(ast);
    assert(def);
    stmt->column_define_list_.push_back(def);
    ast = ast->Next();
  }
  return stmt;
}

ASTBase *ParserContext::NewCreateAsStmt(ASTBase *table_name,
                                        ASTBase * sub_select) {
  ReferenceName *name = dynamic_cast<ReferenceName*>(table_name);
  assert(name);
  SelectStmt *select_stmt = dynamic_cast<SelectStmt*>(sub_select);
  CreateAsStmt *stmt = new CreateAsStmt(name->String(), select_stmt);
  return stmt;
}

ASTBase *ParserContext::NewLoadStmt(ASTBase *table_name,
                                    ASTBase *opt_column_name_list,
                                    ASTBase *file_path) {
  LoadStmt* load_stmt = new LoadStmt();
  ast_node_list_.push_back(load_stmt);
  ReferenceName *name = dynamic_cast<ReferenceName*>(table_name);
  assert(name);
  load_stmt->table_name_ = name->String();
  ASTBase *ast = opt_column_name_list;
  while (ast) {
    ReferenceName *column_name = dynamic_cast<ReferenceName*>(ast);
    assert(column_name);
    load_stmt->opt_column_name_list_.push_back(column_name->String());
    ast = ast->Next();
  }
  ConstValue *const_value = dynamic_cast<ConstValue*>(file_path);
  assert(const_value == file_path);
  if (const_value == NULL) {
    load_stmt->from_stdin_ = true;
  } else {
    load_stmt->file_path_ = const_value->GetStringValue();
  }
  return load_stmt;
}

ASTBase *ParserContext::NewExportStmt(ASTBase *table_name,
                                      ASTBase *opt_column_name_list,
                                      ASTBase *file_path) {
  ExportStmt* export_stmt = new ExportStmt();
  ast_node_list_.push_back(export_stmt);
  ReferenceName *name = dynamic_cast<ReferenceName*>(table_name);
  assert(name);
  export_stmt->table_name_ = name->String();
  ASTBase *ast = opt_column_name_list;
  while (ast) {
    ReferenceName *column_name = dynamic_cast<ReferenceName*>(ast);
    assert(column_name);
    export_stmt->opt_column_name_list_.push_back(column_name->String());
    ast = ast->Next();
  }
  ConstValue *const_value = dynamic_cast<ConstValue*>(file_path);
  assert(const_value == file_path);
  if (const_value == NULL) {
    export_stmt->to_stdout_ = true;
  } else {
    export_stmt->file_path_ = const_value->GetStringValue();
  }
  return export_stmt;
}

ASTBase *ParserContext::NewInsertStmt(ASTBase *table_name,
                                      ASTBase *opt_column_name_list,
                                      ASTBase *value_list) {
  InsertStmt *stmt = new InsertStmt;
  ast_node_list_.push_back(stmt);
  ReferenceName *name = dynamic_cast<ReferenceName*>(table_name);
  assert(name);
  stmt->table_name_ = name->String();
  ASTBase *ast = opt_column_name_list;
  while (ast) {
    ReferenceName *column_name = dynamic_cast<ReferenceName*>(ast);
    assert(column_name);
    stmt->opt_column_names_.push_back(column_name->String());
    ast = ast->Next();
  }

  ast = opt_column_name_list;
  while (ast) {
    ConstValue *const_value = dynamic_cast<ConstValue*>(ast);
    assert(const_value);
    stmt->insert_values_.push_back(const_value);
    ast = ast->Next();
  }
  return stmt;
}

ASTBase * ParserContext::NewSelectStmt(ASTBase* sel_list, ASTBase* tbl_ref_list,
                                       ASTBase* opt_where, ASTBase* opt_group,
                                       ASTBase* opt_have, ASTBase* opt_order) {
  SelectStmt *stmt = new SelectStmt;
  ast_node_list_.push_back(stmt);

  ASTBase * ast = sel_list;
  while (ast) {
    SelectTarget *select_target = dynamic_cast<SelectTarget *>(ast);
    assert(select_target);
    stmt->select_list_.push_back(select_target);
    ast = ast->Next();
  }

  ast = tbl_ref_list;
  while (ast) {
    TableFactor *table_factor = dynamic_cast<TableFactor*>(ast);
    assert(table_factor);
    stmt->table_reference_.push_back(table_factor);
    ast = ast->Next();
  }

  ast = opt_where;
  while (ast) {
    ExpressionBase *expr = dynamic_cast<ExpressionBase *>(ast);
    assert(expr);
    stmt->opt_where_.push_back(expr);
    ast = ast->Next();
  }

  ast = opt_group;
  while (ast) {
    ExpressionBase *expr = dynamic_cast<ExpressionBase *>(ast);
    assert(expr);
    stmt->opt_group_.push_back(expr);
    ast = ast->Next();
  }

  ast = opt_order;
  while (ast) {
    OrderClause *order_by = dynamic_cast<OrderClause *>(ast);
    assert(order_by);
    stmt->opt_order_.push_back(order_by);
    ast = ast->Next();
  }

  return stmt;
}

ASTBase *ParserContext::NewColumnReference(ASTBase* opt_table_name,
                                           ASTBase* column_name) {

  std::string tname;
  std::string cname;
  assert(column_name);
  if (opt_table_name) {
    ReferenceName *name = dynamic_cast<ReferenceName*>(opt_table_name);
    assert(name);
    tname = name->String();
  }

  ReferenceName *name = dynamic_cast<ReferenceName*>(column_name);
  assert(name);
  cname = name->String();
  ColumnReference *ref = new ColumnReference(tname, cname);
  ast_node_list_.push_back(ref);
  return ref;
}

ASTBase *ParserContext::NewSelectTarget(ASTBase* expr, ASTBase* alias_name) {

  ExpressionBase *select_expr = dynamic_cast<ExpressionBase*>(expr);
  assert(select_expr);
  std::string alias;
  if (alias_name) {
    ReferenceName *name = dynamic_cast<ReferenceName*>(alias_name);
    assert(name);
    alias = name->String();
  }
  SelectTarget *select_target = new SelectTarget(select_expr);
  select_target->SetAliasName(alias);
  ast_node_list_.push_back(select_target);
  return select_target;
}

ASTBase *ParserContext::NewReferenceName(const char *str) {
  std::string s = str;
  boost::to_upper(s);
  ReferenceName *nm = new ReferenceName(s);
  ast_node_list_.push_back(nm);
  return nm;
}

ASTBase *ParserContext::NewConstValue(const char *str) {
  ConstValue *val = new ConstValue(str);
  ast_node_list_.push_back(val);
  return val;
}

ASTBase *ParserContext::NewConstValue(int64_t int_val) {
  ConstValue *val = new ConstValue(int_val);
  ast_node_list_.push_back(val);
  return val;
}

ASTBase * ParserContext::NewOrderClause(ASTBase *expr, OrderType ord) {
  ExpressionBase *order_expr = dynamic_cast<ExpressionBase*>(expr);
  OrderClause *orderby = new OrderClause(order_expr, ord == kDesc);
  ast_node_list_.push_back(orderby);
  return orderby;
}

ASTBase *ParserContext::NewTableReference(ASTBase* table_name, ASTBase* alias) {
  ReferenceName *tname = dynamic_cast<ReferenceName*>(table_name);
  ReferenceName *aname = dynamic_cast<ReferenceName*>(table_name);
  assert(tname);
  TableReference *table_ref = new TableReference(tname->String());
  ast_node_list_.push_back(table_ref);
  if (aname != NULL) {
    table_ref->SetAliasName(aname->String());
  }
  return table_ref;
}

ASTBase * ParserContext::NewSubquery(ASTBase* select_stmt, ASTBase* alias) {
  std::string alias_name;
  if (alias) {
    alias_name = dynamic_cast<ReferenceName*>(alias)->String();
  }
  SubQuery *sub_query = new SubQuery(dynamic_cast<SelectStmt*>(select_stmt),
                                     alias_name);
  ast_node_list_.push_back(sub_query);
  return sub_query;
}

ASTBase * ParserContext::NewJoinClause(ASTBase *left, ASTBase *right,
                                       ASTBase *cond, JoinType type) {

  TableFactor *left_expr = dynamic_cast<TableFactor*>(left);
  TableFactor *right_expr = dynamic_cast<TableFactor*>(right);
  ExpressionBase *join_expr = dynamic_cast<ExpressionBase*>(cond);

  JoinClause *join_clause = new JoinClause(left_expr, right_expr, join_expr,
                                           type);
  ast_node_list_.push_back(join_clause);
  return join_clause;
}

ASTBase *ParserContext::NewColumnDefine(ASTBase *column_name,
                                        int32_t type_and_length) {
  ReferenceName *ref_name = dynamic_cast<ReferenceName*>(column_name);
  assert(ref_name);
  ColumnDefine *col_def = new ColumnDefine(
      ref_name->String(), (DataType) LOW_INT16(type_and_length),
      HIGH_INT16(type_and_length));
  ast_node_list_.push_back(col_def);
  return col_def;
}

ASTBase *ParserContext::NewExpression(OperatorType op, ASTBase *left,
                                      ASTBase *right) {
  Operation *expression = new Operation(dynamic_cast<ExpressionBase*>(left),
                                        dynamic_cast<ExpressionBase*>(right),
                                        op);
  ast_node_list_.push_back(expression);
  return expression;
}

ASTBase *ParserContext::NewUpdateStmt(ASTBase *table_name,
                                      ASTBase * set_clause_list,
                                      ASTBase * opt_from_clause,
                                      ASTBase * opt_where) {
  UpdateStmt *update_stmt = new UpdateStmt();
  ast_node_list_.push_back(update_stmt);
  update_stmt->table_name_ = dynamic_cast<ReferenceName*>(table_name)->String();

  ASTBase * ast = set_clause_list;
  ast = opt_where;
  while (ast) {
    Operation *expr = dynamic_cast<Operation*>(ast);
    update_stmt->set_clause_list_.push_back(expr);
    ast->Next();
  }

  ast = opt_from_clause;
  while (ast) {
    TableFactor *table_factor = dynamic_cast<TableFactor*>(ast);
    assert(table_factor);
    update_stmt->opt_from_clause_.push_back(table_factor);
    ast->Next();
  }

  ast = opt_where;
  while (ast) {
    ExpressionBase *expr = dynamic_cast<ExpressionBase*>(ast);
    update_stmt->opt_where_.push_back(expr);
    ast->Next();
  }
  return update_stmt;
}

const std::string & ParserContext::ErrorMessage() {
  return driver_.ErrorMessage();
}
