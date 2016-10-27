#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <map>
#include <vector>
#include <ostream>
#include <stdexcept>
#include <cmath>
#include <string>

#include "parser/ast_base.h"
#include "driver.h"
#include "select_stmt.h"

using std::vector;
using std::string;

/** Calculator context used to save the parsed expressions. This context is
 * passed along to the parser::Driver class and fill during parsing via bison
 * actions. */
class ParserContext {
 public:
  ParserContext() {
    root_ = NULL;
  }

  ~ParserContext();

  ASTBase *new_create_table_stmt(const char * tbl_name, table_scope_t scope,
                                 ASTBase * col_def);

  ASTBase *new_create_table_as_stmt(const char * tbl_name, table_scope_t scope,
                                    ASTBase * col_names, ASTBase *subquery);

  ASTBase *new_insert_stmt(const char* tbl_name, ASTBase *col_names,
                           ASTBase *vals);

  ASTBase *new_select_stmt(ASTBase* sel_list, ASTBase* tbl_ref_list,
                           ASTBase* opt_where, ASTBase* opt_group,
                           ASTBase* opt_have, ASTBase* opt_order);

  ASTBase *new_name(const char* str);

  ASTBase *new_orderby(ASTBase *expr, order_t ord);

  ASTBase *new_table_factor(const char* tbl, const char* alias, ASTBase *sub);

  ASTBase *new_join_table(ASTBase *left, ASTBase *right, ASTBase *cond,
                          join_type_t type);

  ASTBase *new_column_def(const char *col_name, data_type_t type);

  /// type of the variable storage
  typedef std::map<std::string, double> variablemap_type;

  /// variable storage. maps variable string to doubles
  variablemap_type variables;

  ASTBase *root_;

  std::vector<ASTBase*> ast_node_list_;

  parser::Driver driver_;
};

//#define YYSTYPE (union _YYSTYPE)

#endif // EXPRESSION_H
