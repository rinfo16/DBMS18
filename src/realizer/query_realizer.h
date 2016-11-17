#ifndef QUERY_REALIZER_H__
#define QUERY_REALIZER_H__

#include "realizer/query_realizer_interface.h"
#include "realizer/row_output_interface.h"
#include <string>
#include <boost/ptr_container/ptr_vector.hpp>
#include "parser/select_stmt.h"
#include "parser/create_stmt.h"
#include "parser/load_stmt.h"
#include "parser/parser_service_interface.h"
#include "executor/exec_interface.h"
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
	bool ExecSelect();
	bool BuildSelect(SelectStmt *select_stmt);
	bool ExecLoad(LoadStmt *load_stmt);
	bool ExecCreate(CreateStmt *create_stmt);

	std::string sql_stmt_;
	std::string message_;
	RowOutputInterface *output_;
	parser::SQLParserInterface *parser_;
	storage::StorageServiceInterface *storage_;
	ASTBase *parse_tree_;
	RowDesc output_row_desc_;
	std::vector<storage::IteratorInterface*> all_iter_;
	boost::ptr_vector<executor::ExecInterface> all_exec_obj_;
	executor::ExecInterface *top_exec_;
};

} // end namespace realizer

#endif // QUERY_REALIZER_H__

