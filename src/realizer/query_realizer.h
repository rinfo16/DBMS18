#ifndef QUERY_REALIZER_H__
#define QUERY_REALIZER_H__

#include "realizer/query_realizer_interface.h"
#include "realizer/row_output_interface.h"
#include <string>
#include <map>
#include <boost/ptr_container/ptr_vector.hpp>
#include "common/row_desc.h"
#include "parser/operation.h"
#include "parser/select_stmt.h"
#include "parser/create_stmt.h"
#include "parser/load_stmt.h"
#include "parser/const_value.h"
#include "parser/parser_service_interface.h"
#include "parser/column_reference.h"
#include "parser/table_reference.h"
#include "executor/exec_interface.h"
#include "executor/slot_reference.h"
#include "storage/storage_service_interface.h"
#include "executor/boolean_expr_interface.h"
namespace realizer {

class QueryRealizer : public QueryRealizerInterface {
 public:
  QueryRealizer(RowOutputInterface* row_output, const std::string & sql);
  virtual ~QueryRealizer();
  virtual State Parse();
  virtual State Check();
  virtual State Optimize();
  virtual State Build();
  virtual State Execute();
  virtual std::string Message() const;
 private:

  enum ClauseType {
    kSelect,
    kWhere,
    kJoinOn,
    kOrderBy,
    kGroupBy,
    kHaving
  };

  bool BuildSelectStmt(ast::SelectStmt *select_stmt);
  executor::ExecInterface* BuildJoin(
      const std::vector<ast::TableFactor *> table_factor_list);
  executor::ExecInterface* BuildJoin(const ast::TableFactor *table_factor);
  executor::ExecInterface* BuildTableScan(
      const ast::TableReference *table_reference);
  executor::ExecInterface* BuildSelect(const ast::ExpressionBase* opt_where);
  executor::ExecInterface* BuildProjection(const std::vector<ast::SelectTarget*> & select_list);
  executor::BooleanExprInterface *BuildBooleanExpression(
      const ast::ExpressionBase *operation);
  executor::ValueExprInterface *BuildValueExpression(
      const ast::ExpressionBase *expr_base);

  bool ExecCreate(ast::CreateStmt *create_stmt);
  bool ExecLoad(ast::LoadStmt *load_stmt);
  bool LoadFromFile(const std::string table, const std::string path);
  State ExecSelect();
  State CheckFrom(const ast::SelectStmt *select_stmt);
  State CheckWhere(const ast::SelectStmt *select_stmt);
  State CheckGroupBy(const ast::SelectStmt *select_stmt);
  State CheckSelect(const ast::SelectStmt *select_stmt);
  State CheckOrderBy(const ast::SelectStmt *select_stmt);
  State CheckTableFactor(const ast::TableFactor *table_factor);
  State CheckExpressionBase(const ast::ExpressionBase *expr_base,
                            ClauseType type);
  std::string sql_stmt_;
  std::string message_;
  RowOutputInterface *output_;
  parser::SQLParserInterface *parser_;
  storage::StorageServiceInterface *storage_;
  ast::ASTBase *parse_tree_;
  RowDesc output_row_desc_;
  executor::ExecInterface *top_exec_;

  // table name , column name
  typedef std::pair<std::string, std::string> ColumnRefName;
  std::map<std::string, Relation*> name2relation_;
  std::map<std::string, int32_t> name2tuple_index_;
  std::map<const ast::ColumnReference*, executor::SlotReference> name2slot_;

  std::vector<executor::ValueExprInterface*> projection_list_;

  std::vector<const ast::ColumnReference*> column_reference_;

  std::vector<const ast::ColumnReference*> select_ref_;
  std::vector<const ast::ColumnReference*> opt_group_by_ref_;
  // TODO there is no order by expression
  // col_ref_order_by_.size() == order_by_is_desc_.size()
  std::vector<const ast::ColumnReference*> opt_order_by_ref_;
  std::vector<bool> opt_order_by_is_desc_;
  std::vector<const ast::Operation*> opt_where_;

  std::vector<RowDesc> all_tuple_desc_;
  boost::ptr_vector<executor::DatumInterface> all_datum_items_;
  std::vector<storage::IteratorInterface*> all_iter_;
  boost::ptr_vector<executor::ExecInterface> all_exec_obj_;
};

}  // end namespace realizer

#endif // QUERY_REALIZER_H__

