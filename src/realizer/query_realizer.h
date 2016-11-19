#ifndef QUERY_REALIZER_H__
#define QUERY_REALIZER_H__

#include "realizer/query_realizer_interface.h"
#include "realizer/row_output_interface.h"
#include <string>
#include <map>
#include <boost/ptr_container/ptr_vector.hpp>
#include "parser/select_stmt.h"
#include "parser/create_stmt.h"
#include "parser/load_stmt.h"
#include "parser/const_value.h"
#include "parser/expression.h"
#include "parser/parser_service_interface.h"
#include "parser/column_reference.h"
#include "executor/exec_interface.h"
#include "executor/slot_reference.h"
#include "storage/storage_service_interface.h"

namespace realizer{

class QueryRealizer : public QueryRealizerInterface
{
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
	bool BuildSelect(ast::SelectStmt *select_stmt);
  bool BuildJoin(const std::vector<ast::TableFactor *> table_factor_list);
  bool BuildJoin(ast::TableFactor *table_factor);
	bool ExecCreate(ast::CreateStmt *create_stmt);
	bool ExecLoad(ast::LoadStmt *load_stmt);
  bool ExecSelect();
  State CheckSelect(ast::SelectStmt *select_stmt);

	std::string sql_stmt_;
	std::string message_;
	RowOutputInterface *output_;
	parser::SQLParserInterface *parser_;
	storage::StorageServiceInterface *storage_;
	ast::ASTBase *parse_tree_;
	RowDesc output_row_desc_;
	std::vector<storage::IteratorInterface*> all_iter_;
	boost::ptr_vector<executor::ExecInterface> all_exec_obj_;
	executor::ExecInterface *top_exec_;

	std::map<std::string, Relation*> table2relation_;
	std::map<ast::ColumnReference*, SlotReference> column2slot_;
	std::vector<ast::ColumnReference*> column_reference_;
  std::vector<ast::ConstValue*> const_value_;
	std::vector<ast::Expression*> cal_expression_;
  std::vector<ast::Expression*> join_predicate_;
  std::vector<ast::Expression*> where_predicate_;
};

} // end namespace realizer

#endif // QUERY_REALIZER_H__

