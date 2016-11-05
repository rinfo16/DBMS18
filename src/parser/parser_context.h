#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <map>
#include <vector>
#include <ostream>
#include <stdexcept>
#include <cmath>
#include <string>

#include "parser/ast_base.h"
#include "parser/ast_def.h"
#include "driver.h"

using std::vector;
using std::string;

/** Parser context used to save the parsed SQL. This context is
 * passed along to the parser::Driver class and fill during parsing via bison
 * actions. */
class ParserContext {
 public:
  ParserContext(const std::string &sql)
      : sql_(sql) {
    root_ = NULL;
  }

  ~ParserContext();

  bool Parse();

  ASTBase *GetAST();

  ASTBase *NewCreateStmt(ASTBase *table_name, ASTBase * column_define_list);

  ASTBase *NewCreateAsStmt(ASTBase *table_name, ASTBase * select_stmt);

  ASTBase *NewLoadStmt(ASTBase *table_name, ASTBase *opt_column_name_list,
                         ASTBase *file_path);

  ASTBase *NewInsertStmt(ASTBase *table_name, ASTBase *opt_column_name_list,
                         ASTBase *value_list);

  ASTBase *NewSelectStmt(ASTBase* sel_list, ASTBase* tbl_ref_list,
                         ASTBase* opt_where, ASTBase* opt_group,
                         ASTBase* opt_have, ASTBase* opt_order);

  ASTBase *NewColumnReference(ASTBase* opt_table_name, ASTBase* column_name);

  ASTBase *NewSelectTarget(ASTBase* expr_or_ref, ASTBase* alias_name);

  ASTBase *NewOrderClause(ASTBase *expr, OrderType ord);

  ASTBase *NewTableReference(ASTBase* table_name, ASTBase* alias);

  ASTBase *NewSubquery(ASTBase* select_stmt, ASTBase* alias);

  ASTBase *NewJoinClause(ASTBase *left, ASTBase *right, ASTBase *cond,
                         JoinType type);

  ASTBase *NewExpression(OperatorType op, ASTBase *left, ASTBase *right);

  ASTBase *NewUpdateStmt(ASTBase *table_name, ASTBase * set_clause_list,
                         ASTBase * opt_from_clause, ASTBase * opt_where);

  ASTBase *NewReferenceName(const char* str);

  ASTBase *NewConstValue(const char *str_val);

  ASTBase *NewConstValue(int64_t int_val);

  ASTBase *NewColumnDefine(ASTBase *column_name, int32_t type_and_length);

  /// type of the variable storage
  typedef std::map<std::string, double> variablemap_type;

  /// variable storage. maps variable string to doubles
  variablemap_type variables;

  ASTBase *root_;

  std::vector<ASTBase*> ast_node_list_;

  parser::Driver driver_;

  std::string sql_;
};

//#define YYSTYPE (union _YYSTYPE)

#endif // EXPRESSION_H
