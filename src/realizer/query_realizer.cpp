#include "query_realizer.h"
#include "parser/table_reference.h"
#include "parser/column_reference.h"
#include "parser/create_stmt.h"
#include "executor/seq_scan.h"
namespace realizer {

QueryRealizer::QueryRealizer(RowOutputInterface *output,
		const std::string & sql) :
		output_(output), parser_(NULL), parse_tree_(NULL), top_exec_(NULL) {
	storage_ = storage::StorageServiceInterface::Instance();
}

QueryRealizer::~QueryRealizer() {
	parser::ParserServiceInterface::Instance()->DestroySQLParser(parser_);
	for (size_t i = 0; i < all_iter_.size(); i++) {
		storage_->DeleteIOObject(all_iter_[i]);
	}
}

State QueryRealizer::Parse() {
	BOOST_LOG_TRIVIAL(debug)<< sql_stmt_;
	parser_ = parser::ParserServiceInterface::Instance()->CreateSQLParser(sql_stmt_);
	parse_tree_ = parser_->Parse();
	if (parse_tree_ == NULL) {
		message_ = "parse error";
		return kStateParseError;
	}
	return kStateOK;
}

State QueryRealizer::Check() {
	// TODO .. semantic check
	return kStateOK;
}

State QueryRealizer::Optimize() {
	// TODO .. reduce sql
	// TODO .. optimize path
	// TODO .. CBO
	return kStateOK;
}

State QueryRealizer::Build() {
	bool ok = true;
	if (parse_tree_->ASTType() == kASTSelectStmt) {
		SelectStmt *select_stmt = dynamic_cast<SelectStmt *>(parse_tree_);
		ok = BuildSelect(select_stmt);
	}
	if (ok) {
		return kStateOK;
	} else {
		return kStateExecError;
	}
}

State QueryRealizer::Execute() {
	bool ok = false;
	if (parse_tree_->ASTType() == kASTCreateStmt) {
		CreateStmt *create_stmt = dynamic_cast<CreateStmt *>(parse_tree_);
		ok = ExecCreate(create_stmt);
	} else if (parse_tree_->ASTType() == kASTCopyStmt) {
		LoadStmt *load_stmt = dynamic_cast<LoadStmt *>(parse_tree_);
		ok = ExecLoad(load_stmt);
	} else if (parse_tree_->ASTType() == kASTSelectStmt) {
		SelectStmt *select_stmt = dynamic_cast<SelectStmt *>(parse_tree_);
		ok = ExecSelect();
	}
	if (ok) {
		return kStateOK;
	} else {
		return kStateExecError;
	}
}

std::string QueryRealizer::Message() const {
	return message_;
}

bool QueryRealizer::BuildSelect(SelectStmt *select_stmt) {
	bool ret = false;
	storage::IteratorInterface *iter = NULL;
	std::string table_name;
	auto table_factor_list = select_stmt->TableReference();
	const TableFactor * table_factor = table_factor_list[0];
	if (table_factor->ASTType() == kASTTableReference) {
		const TableReference *table_reference =
				dynamic_cast<const TableReference*>(table_factor);
		table_name = table_reference->TableName();
	} else {
		// NOT SUPPORT TODO
		assert(false);
	}

	Relation *relation = storage_->GetMetaDataManager()->GetRelationByName(
			table_name);
	if (relation == NULL) {
		return false;
	}
	std::vector<uint32_t> projection_mapping;
	auto select_list = select_stmt->SelectList();
	for (size_t i = 0; i < select_list.size(); i++) {
		const ExpressionBase *expr = select_list[i]->SelectExpression();
		if (expr->ASTType() == kASTColumnReference) {
			const ColumnReference *column_reference =
					dynamic_cast<const ColumnReference*>(expr);
			int32_t index = relation->GetAttributeIndex(
					column_reference->ColumnName());
			if (index < 0)
				return false;
			projection_mapping.push_back(index);
		}
	}

	iter = storage_->NewIterator(table_name);
	if (iter == NULL) {
		return false;
	}
	all_iter_.push_back(iter);

	output_row_desc_ = relation->ToDesc();
	BOOST_LOG_TRIVIAL(debug)<< "build execution tree";
	// begin build execution tree ...

	executor::ExecInterface *exec = new executor::SeqScan(iter, 0);
	all_exec_obj_.push_back(exec);
	top_exec_ = exec;
}

bool QueryRealizer::ExecLoad(LoadStmt *load_stmt) {
	const std::string & table_name = load_stmt->TableName();
	const std::string & file_path = load_stmt->FilePath();
	bool ok = storage_->Load(table_name, file_path);
	std::string msg;
	if (ok) {
		message_ = "copy " + table_name + " from '" + file_path + "' success.";
	} else {
		message_ = "copy " + table_name + " from '" + file_path + "' failed.";
	}
	return ok;
}

bool QueryRealizer::ExecCreate(CreateStmt *create_stmt) {
	TableSchema schema;
	schema.name_ = create_stmt->TableName();
	const std::vector<ColumnDefine*> &def_list =
			create_stmt->ColumnDefineList();
	for (int i = 0; i < def_list.size(); i++) {
		ColumnDefine *column_define = def_list[i];
		ColumnSchema column_schema;
		column_schema.name_ = column_define->ColumnName();
		column_schema.data_type_ = column_define->ColumnType();
		column_schema.length_ = column_define->DataLength();
		column_schema.is_null_ = false;
		schema.column_list_.push_back(column_schema);
	}
	bool ok = storage_->CreateRelation(schema);
	if (ok) {
		message_ = ("table " + schema.name_ + " created.");
	} else {
		message_ = ("table " + schema.name_ + " create failed.");
	}
	return ok;
}

bool QueryRealizer::ExecSelect() {
	TupleRow *row = NULL;
	output_->SendRowDescription(&output_row_desc_);
	bool ret = false;
	std::stringstream ssm;
	int rows = 0;
	BOOST_LOG_TRIVIAL(debug)<< "execute the plan";
	// execute the plan
	if (!top_exec_->Prepare()) {
		goto RET;
	}
	if (!top_exec_->Open()) {
		goto RET;
	}

	row = new TupleRow(1);
	while (true) {
		bool ok = top_exec_->GetNext(row);
		if (!ok)
			break;
		rows++;
		output_->SendRowData(row, &output_row_desc_);
	}
	top_exec_->Close();
	ssm << "SELECT " << rows << " rows." << std::endl;
	//SendCommandComplete(ssm.str());
	ret = true;

	RET: delete row;
	return ret;
}

QueryRealizerInterface* NewRealizer(
		RowOutputInterface *output, const std::string & sql) {
	return new QueryRealizer(output, sql);
}

void DeleteRealizer(
		QueryRealizerInterface* realizer) {
	delete realizer;
}
} // end namespace realizer
