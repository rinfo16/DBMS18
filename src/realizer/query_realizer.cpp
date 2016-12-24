#include "query_realizer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "parser/table_reference.h"
#include "parser/column_reference.h"
#include "parser/create_stmt.h"
#include "parser/join_clause.h"
#include "executor/seq_scan.h"
#include "executor/nested_loop_join.h"
#include "executor/slot_reference.h"
#include "executor/project.h"
#include "executor/export.h"
#include "executor/load.h"
#include "executor/select.h"
#include "executor/compare.h"
#include "executor/const_value.h"
#include "executor/logic_expr.h"

namespace realizer {

QueryRealizer::QueryRealizer(RowOutputInterface *output,
                             const std::string & sql)
    : output_(output),
      parser_(NULL),
      parse_tree_(NULL),
      top_exec_(NULL),
      top_cmd_(NULL),
      sql_stmt_(sql) {
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
  parse_tree_ = parser_->Parse(message_);
  if (parse_tree_ == NULL) {
    return kStateParseError;
  }
  return kStateOK;
}

State QueryRealizer::Check() {
  // TODO .. semantic check
  State state;
  if (parse_tree_->ASTType() == kASTSelectStmt) {
    ast::SelectStmt *select_stmt = dynamic_cast<ast::SelectStmt *>(parse_tree_);
    state = CheckFrom(select_stmt);
    if (state != kStateOK)
      return state;
    state = CheckWhere(select_stmt);
    if (state != kStateOK)
      return state;
    state = CheckGroupBy(select_stmt);
    if (state != kStateOK)
      return state;

    state = CheckSelect(select_stmt);
    if (state != kStateOK)
      return state;

    state = CheckOrderBy(select_stmt);
    if (state != kStateOK)
      return state;
  }
  return kStateOK;
}

State QueryRealizer::CheckFrom(const ast::SelectStmt *select_stmt) {
  auto table_factor_list = select_stmt->TableReference();
  for (int i = 0; i < table_factor_list.size(); i++) {
    State state = CheckTableFactor(table_factor_list[i]);
    if (state != kStateOK)
      return state;
  }
  return kStateOK;
}

State QueryRealizer::CheckTableFactor(const ast::TableFactor *table_factor) {

  if (table_factor->ASTType() == kASTTableReference) {
    const ast::TableReference *table_reference =
        dynamic_cast<const ast::TableReference*>(table_factor);
    Relation *relation = storage_->GetRelation(
        table_reference->TableName());
    if (relation == NULL) {
      message_ = "cannot find relation [" + table_reference->TableName() + "].";
      return kStateRelationNotFound;
    }
    std::string table_name;
    if (!table_reference->AliasName().empty()) {
      table_name = table_reference->AliasName();
    } else {
      table_name = table_reference->TableName();
    }
    name2tuple_index_.insert(
        std::make_pair(table_name, name2tuple_index_.size()));
    name2relation_.insert(std::make_pair(table_name, relation));
  } else if (table_factor->ASTType() == kASTJoinClause) {
    const ast::JoinClause *join_clause =
        dynamic_cast<const ast::JoinClause*>(table_factor);
    State state = CheckTableFactor(join_clause->LeftTable());
    if (state != kStateOK)
      return state;

    state = CheckTableFactor(join_clause->RightTable());
    if (state != kStateOK)
      return state;

    state = CheckExpressionBase(join_clause->JoinPredicate(), kJoinOn);
    if (state != kStateOK)
      return state;
  }
  return kStateOK;
}

State QueryRealizer::CheckExpressionBase(const ast::ExpressionBase *expr_base,
                                         ClauseType type) {
  if (expr_base->ASTType() == kASTColumnReference) {
    const ast::ColumnReference *ref =
        static_cast<const ast::ColumnReference*>(expr_base);
    Relation *rel = NULL;
    int32_t slot_index = 0;
    ColumnRefName name;
    std::string column_name = ref->ColumnName();
    if (!ref->TableName().empty()) {
      auto iter = name2relation_.find(ref->TableName());
      if (iter == name2relation_.end()) {
        message_ = "cannot find table name [" + ref->TableName() + "]";
        return kStateRelationNotFound;
      }
      rel = iter->second;
      name.first = iter->first;
      slot_index = rel->GetAttributeIndex(ref->ColumnName());
      if (slot_index < 0) {
        message_ = "cannot find attribute name [" + ref->ColumnName() + "]";
        return kStateAttributeNotFound;
      }
    } else {
      std::vector<uint32_t> attr_vec;
      for (auto iter = name2relation_.begin(); iter != name2relation_.end();
          ++iter) {
        rel = iter->second;
        int32_t i = rel->GetAttributeIndex(ref->ColumnName());
        if (i >= 0) {  // OK, we find the right attribute
          attr_vec.push_back(i);
          name.first = iter->first;
        }
      }
      if (attr_vec.size() > 1) {
        message_ = "attribute [" + ref->ColumnName() + "] ambiguous.";
        return kStateNameAmbiguous;
      } else if (attr_vec.empty()) {
        message_ = "cannot find attribute name [" + ref->ColumnName() + "]";
        return kStateAttributeNotFound;
      }
      assert(attr_vec.size() == 1);
      slot_index = attr_vec[0];
    }
    assert(name2tuple_index_.find(name.first) != name2tuple_index_.end());
    executor::SlotReference slot_ref(name2tuple_index_[name.first], slot_index);
    const Attribute & attr = rel->GetAttribute(slot_index);
    slot_ref.SetDataType(attr.GetDataType());
    name2slot_.insert(std::make_pair(ref, slot_ref));
    if (type == kSelect) {
      select_ref_.push_back(ref);
    } else if (type == kGroupBy) {
      opt_group_by_ref_.push_back(ref);
    } else if (type == kOrderBy) {
      opt_order_by_ref_.push_back(ref);
    } else if (type == kJoinOn) {
    } else if (type == kWhere) {
    } else {
      assert(false);
    }
  } else if (expr_base->ASTType() == kASTOperation) {
    //TODO
    assert(type == kJoinOn || type == kWhere);
    const ast::Operation *operation =
        static_cast<const ast::Operation*>(expr_base);

    // check the left operate number
    State state = CheckExpressionBase(operation->Left(), type);
    if (state != kStateOK)
      return state;

    // check the right operate number
    state = CheckExpressionBase(operation->Right(), type);
    if (state != kStateOK)
      return state;

    if (type == kWhere) {
      opt_where_.push_back(operation);
    }
  } else if (expr_base->ASTType() == kASTOperation) {
    if (type != kWhere) {
      // const value expression
      message_ = "unsupported expression";
      return kStateNotSupport;
    }
  }
  return kStateOK;
}

State QueryRealizer::CheckWhere(const ast::SelectStmt *select_stmt) {
  auto opt_where = select_stmt->OptWhere();
  for (size_t i = 0; i < opt_where.size(); i++) {
    State state = CheckExpressionBase(opt_where[i], kWhere);
    if (state != kStateOK)
      return state;
  }
  return kStateOK;
}

State QueryRealizer::CheckGroupBy(const ast::SelectStmt *select_stmt) {
  auto opt_group_by = select_stmt->OptGroupBy();
  for (size_t i = 0; i < opt_group_by.size(); i++) {
    State state = CheckExpressionBase(opt_group_by[i], kGroupBy);
    if (state != kStateOK)
      return state;
  }
  return kStateOK;
}

State QueryRealizer::CheckSelect(const ast::SelectStmt *select_stmt) {
  auto select_target = select_stmt->SelectList();
  for (auto i = 0; i < select_target.size(); i++) {
    const ast::SelectTarget *t = select_target[i];
    const ast::ExpressionBase *select_expr = t->SelectExpression();
    State state = CheckExpressionBase(select_expr, kSelect);
    if (state != kStateOK)
      return state;
  }
  return kStateOK;
}

State QueryRealizer::CheckOrderBy(const ast::SelectStmt *select_stmt) {
  auto opt_order_by = select_stmt->OptOrderBy();
  for (size_t i = 0; i < opt_order_by.size(); i++) {
    opt_order_by_is_desc_.push_back(opt_order_by[i]->IsDesc());
    State state = CheckExpressionBase(opt_order_by[i]->OrderByExpression(),
                                      kOrderBy);
    if (state != kStateOK)
      return state;
  }
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
    ast::SelectStmt *select_stmt = dynamic_cast<ast::SelectStmt *>(parse_tree_);
    top_exec_ = BuildJoin(select_stmt->TableReference());
    if (top_exec_ == NULL)
      return kStateBuildError;
    if (!select_stmt->OptWhere().empty()) {
      top_exec_ = BuildSelect(select_stmt->OptWhere()[0]);
      if (top_exec_ == NULL)
        return kStateBuildError;
    }
    top_exec_ = BuildProjection(select_stmt->SelectList());
    if (top_exec_ == NULL)
      return kStateBuildError;
  }
  return kStateOK;
}

State QueryRealizer::Execute() {
  bool ok = false;
  TreeType type = parse_tree_->ASTType();
  if (type == kASTCreateStmt) {
    ast::CreateStmt *create_stmt = dynamic_cast<ast::CreateStmt *>(parse_tree_);
    ok = ExecCreate(create_stmt);
  } else if (type == kASTCopyFromStmt) {
    ast::LoadStmt *load_stmt = dynamic_cast<ast::LoadStmt *>(parse_tree_);
    return ExecLoad(load_stmt);
  } else if (type == kASTCopyToStmt) {
    ast::ExportStmt *export_stmt = dynamic_cast<ast::ExportStmt *>(parse_tree_);
    return ExecExport(export_stmt);
  } else if (type == kASTSelectStmt) {
    ast::SelectStmt *select_stmt = dynamic_cast<ast::SelectStmt *>(parse_tree_);
    return ExecSelect();
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

executor::ExecInterface* QueryRealizer::BuildJoin(
    const std::vector<ast::TableFactor *> table_factor_list) {
  for (auto i = 0; i < table_factor_list.size(); i++) {
    return BuildJoin(table_factor_list[i]);
  }
  // TODO only support SQL 92 syntax, don not support SQL 89 syntax
  // EG. SQL like: select tbl1.col1, tbl2.col2 from tbl1, tbl2 where
  //               tab1.col1 = tbl2.col1..
  message_ = "unsupported SQL 89 join syntax.";
  return NULL;
}

executor::ExecInterface* QueryRealizer::BuildJoin(
    const ast::TableFactor *table_factor) {
  if (table_factor->ASTType() == kASTTableReference) {
    // table reference, build a table scan executor
    const ast::TableReference *table_reference =
        dynamic_cast<const ast::TableReference*>(table_factor);
    return BuildTableScan(table_reference);
  } else if (table_factor->ASTType() == kASTJoinClause) {
    // this is a join clause, SQL 92 syntax
    const ast::JoinClause *join_clause =
        dynamic_cast<const ast::JoinClause*>(table_factor);

    // build the left join executor
    executor::ExecInterface* left_exec = BuildJoin(join_clause->LeftTable());

    // build the right join executor
    executor::ExecInterface* right_exec = BuildJoin(join_clause->RightTable());

    // build join predicate
    if (join_clause->JoinPredicate()->ASTType() != kASTOperation) {
      // join predicate must be an operation expression
      assert(false);
      message_ = "fatal error, build join error.";
      return NULL;
    }
    const ast::Operation *operation =
        static_cast<const ast::Operation*>(join_clause->JoinPredicate());
    executor::BooleanExprInterface *boolean_expr = BuildBooleanExpression(
        operation);
    int32_t tuple_count = name2relation_.size();
    executor::ExecInterface* join_exec = new executor::NestedLoopJoin(
        tuple_count, left_exec, right_exec, boolean_expr);
    all_exec_obj_.push_back(join_exec);
    return join_exec;
  }
  assert(false);
  message_ = "fatal error, build join error.";
  return NULL;
}

executor::ExecInterface* QueryRealizer::BuildSelect(
    const ast::ExpressionBase* opt_where) {
  executor::BooleanExprInterface *bool_expr = BuildBooleanExpression(opt_where);
  if (bool_expr == NULL) {
    return NULL;
  }
  executor::ExecInterface *exec = new executor::Select(top_exec_, bool_expr);
  all_exec_obj_.push_back(exec);
  return exec;
}

executor::ExecInterface* QueryRealizer::BuildProjection(
    const std::vector<ast::SelectTarget*> & select_list) {
  for (auto i = 0; i < select_list.size(); i++) {
    const ast::SelectTarget *t = select_list[i];
    const ast::ExpressionBase *select_expr = t->SelectExpression();
    TreeType type = select_expr->ASTType();
    if (type == kASTColumnReference) {
      const ast::ColumnReference *ref =
          static_cast<const ast::ColumnReference*>(select_expr);
      auto iter = name2slot_.find(ref);
      if (iter == name2slot_.end()) {
        message_ = "build projection, fatal error, cannot find name ["
            + ref->ColumnName() + "].";
        return NULL;
      }
      executor::SlotReference *expr = new executor::SlotReference(iter->second);
      projection_list_.push_back(expr);

      assert(all_tuple_desc_.size() > expr->GetTupleIndex());
      const RowDesc & row_desc = all_tuple_desc_[expr->GetTupleIndex()];
      assert(row_desc.GetColumnCount() > expr->GetSlotIndex());
      ColumnDesc col_desc = row_desc.GetColumnDesc(expr->GetSlotIndex());
      output_row_desc_.PushColumnDesc(col_desc);
    } else if (type == kASTOperation) {
    } else if (type == kASTConstValue) {
      // const value expression
    } else {
    }
  }
  executor::ExecInterface *ret = NULL;
  int32_t tuple_count = name2relation_.size();
  ret = new executor::Project(top_exec_, projection_list_, tuple_count);
  all_exec_obj_.push_back(ret);
  return ret;
}

executor::BooleanExprInterface *QueryRealizer::BuildBooleanExpression(
    const ast::ExpressionBase *expression_base) {
  executor::BooleanExprInterface *ret = NULL;
  if (expression_base->ASTType() != kASTOperation) {
    assert(false);
    return NULL;
  }
  const ast::Operation *operation =
      static_cast<const ast::Operation *>(expression_base);
  if (IsCompareOperator(operation->Operator())) {
    executor::ValueExprInterface *left = BuildValueExpression(
        operation->Left());
    executor::ValueExprInterface *right = BuildValueExpression(
        operation->Right());
    if (left->GetDataType() == right->GetDataType()) {
      DataType data_type = left->GetDataType();
      if (data_type == kDTInteger) {
        ret = new executor::IntegerCompare(operation->Operator(), left, right);
        all_datum_items_.push_back(ret);
      } else if (data_type == kDTFloat) {
        ret = new executor::FloatCompare(operation->Operator(), left, right);
        all_datum_items_.push_back(ret);
      } else if (data_type == kDTVarchar) {
        ret = new executor::VarcharCompare(operation->Operator(), left, right);
        all_datum_items_.push_back(ret);
      }
    } else {
      // TODO  type cast ...
    }
    // TODO and/or/xor support...
  } else if (IsLogicOperator(operation->Operator())) {
    const ast::ExpressionBase *left = operation->Left();
    const ast::ExpressionBase *right = operation->Right();
    executor::BooleanExprInterface *bool_left = BuildBooleanExpression(left);
    executor::BooleanExprInterface *bool_right = BuildBooleanExpression(right);
    if (bool_left == NULL || bool_right == NULL) {
      return NULL;
    }
    ret = new executor::LogicExpr(operation->Operator(), bool_left, bool_right);
    all_datum_items_.push_back(ret);
    if (left->ASTType() == kASTColumnReference) {
      //static_cast<const ast::ColumnReference*>(left);
    } else if (left->ASTType() == kASTConstValue) {

    } else if (left->ASTType() == kASTOperation) {

    }
    if (right->ASTType() == kASTColumnReference) {

    }
  }
  return ret;
}

executor::ValueExprInterface *QueryRealizer::BuildValueExpression(
    const ast::ExpressionBase *expr_base) {
  executor::ValueExprInterface *ret = NULL;
  if (expr_base->ASTType() == kASTColumnReference) {
    const ast::ColumnReference* col_ref =
        static_cast<const ast::ColumnReference*>(expr_base);
    auto iter = name2slot_.find(col_ref);
    if (iter == name2slot_.end()) {
      assert(false);
      return NULL;
    }
    ret = new executor::SlotReference(iter->second);
    all_datum_items_.push_back(ret);
  } else if (expr_base->ASTType() == kASTConstValue) {
    const ast::ConstValue* const_value =
        static_cast<const ast::ConstValue*>(expr_base);
    DataType data_type = const_value->GetDataType();
    if (data_type == kDTInteger) {
      ret = new executor::IntegerConstValue(const_value->GetIntegerValue());
      ret->SetDataType(kDTInteger);
      all_datum_items_.push_back(ret);
    } else if (data_type == kDTVarchar) {
      ret = new executor::VarcharConstValue(const_value->GetStringValue());
      ret->SetDataType(kDTVarchar);
      all_datum_items_.push_back(ret);
    }
  } else if (expr_base->ASTType() == kASTOperation) {
    const ast::Operation *operation =
        static_cast<const ast::Operation*>(expr_base);
    executor::ValueExprInterface *left = BuildValueExpression(
        operation->Left());
    executor::ValueExprInterface *right = BuildValueExpression(
        operation->Right());
    // TODO build arithmetic expression
    // ...
  }
  return ret;
}

executor::ExecInterface* QueryRealizer::BuildTableScan(
    const ast::TableReference *table_reference) {
  std::string table_name = table_reference->TableName();
  Relation *rel = storage_->GetRelation(table_name);
  storage::IteratorHandler *iter = storage_->NewIterator(rel->GetID());
  assert(iter);
  if (iter == NULL) {
    return NULL;
  }
  all_iter_.push_back(iter);

  if (!table_reference->AliasName().empty()) {
    table_name = table_reference->AliasName();
  }

  Relation *relation = NULL;
  auto i = name2relation_.find(table_name);
  assert(i != name2relation_.end());
  if (i == name2relation_.end()) {
    return NULL;
  }

  relation = i->second;
  int32_t tuple_index = all_tuple_desc_.size();
  all_tuple_desc_.push_back(relation->ToDesc());
  executor::ExecInterface *exec = new executor::SeqScan(iter, tuple_index);
  all_exec_obj_.push_back(exec);
  return exec;
}

State QueryRealizer::ExecLoad(const ast::LoadStmt *load_stmt) {
  std::string table_name = load_stmt->TableName();
  std::string file_path = load_stmt->FilePath();
  if (load_stmt->IsFromStdin()) {
    std::stringstream ssm;
    ssm << "/tmp/tmpfile_import_" << this << ".csv";
    int32_t columns = 0;
    Relation *rel = storage_->GetRelation(
        table_name);
    if (rel == NULL) {
      message_ = "table not found.";
      return kStateRelationNotFound;
    }
    if (load_stmt->OptColumnNameList().empty()) {
      columns = rel->GetAttributeCount();
    } else {  // TODO load the specified column
      columns = rel->GetAttributeCount();
    }
    output_->SendCopyInResponse(columns);

    file_path = ssm.str();
    std::ofstream ofs(file_path);

    std::string data;
    do {
      data = output_->RecvCopyData();
      ofs << data;
    } while (!data.empty());
    //remove(file_path.c_str());
    ofs.close();
  }
  executor::CmdInterface *cmd = new executor::Load(file_path, table_name);
  all_exec_obj_.push_back(cmd);
  top_cmd_ = cmd;
  return ExecCmd();
}

State QueryRealizer::ExecCmd() {
  assert(top_cmd_);
  State state = top_cmd_->Prepare();
  if (state != kStateOK) {
    message_ = top_cmd_->GetResponse();
    return state;
  }
  state = top_cmd_->Exec();
  if (state != kStateOK) {
    message_ = top_cmd_->GetResponse();
    return state;
  }
  message_ = top_cmd_->GetResponse();
  return state;
}

State QueryRealizer::ExecExport(const ast::ExportStmt *export_stmt) {
  std::string table_name = export_stmt->TableName();
  Relation *rel = storage_->GetRelation(table_name);
  if (rel == NULL) {
    return kStateRelationNotFound;
  }
  storage::IteratorHandler *iter = storage_->NewIterator(rel->GetID());
  if (iter == NULL) {
    return kStateRelationNotFound;;
  }
  all_iter_.push_back(iter);
  executor::ExecInterface *exec = new executor::SeqScan(iter, 0);
  top_exec_ = exec;
  all_exec_obj_.push_back(exec);

  int32_t columns;
  if (export_stmt->OptColumnNameList().empty()) {
    columns = rel->GetAttributeCount();
  } else {  // TODO export the specified column
    columns = rel->GetAttributeCount();
  }

  RowDesc desc = rel->ToDesc();
  std::string file_path = export_stmt->FilePath();
  if (export_stmt->IsToStdout()) {
    std::stringstream ssm;
    ssm << "/tmp/tmpfile_export_" << this << ".csv";
    file_path = ssm.str();
    output_->SendCopyOutResponse(columns);
  } else {
    std::ofstream ofs(file_path);
    std::string data;
  }
  executor::CmdInterface *cmd = new executor::Export(top_exec_, &desc,
                                                     file_path);
  top_cmd_ = cmd;
  all_exec_obj_.push_back(cmd);

  State state = ExecCmd();
  std::stringstream ssm;
  if (state == kStateOK) {
    std::string line;
    std::ifstream fs(file_path);

    while (std::getline(fs, line)) {
      line += '\n';
      output_->SendCopyData(line);
    }
    //remove(file_path.c_str());

    return kStateOK;
  }
  return state;

}

bool QueryRealizer::ExecCreate(ast::CreateStmt *create_stmt) {
  TableSchema schema;
  schema.name_ = create_stmt->TableName();
  const std::vector<ast::ColumnDefine*> &def_list = create_stmt
      ->ColumnDefineList();
  for (int i = 0; i < def_list.size(); i++) {
    ast::ColumnDefine *column_define = def_list[i];
    ColumnSchema column_schema;
    column_schema.name_ = column_define->ColumnName();
    column_schema.data_type_ = column_define->ColumnType();
    column_schema.length_ = column_define->DataLength();
    column_schema.is_null_ = false;
    schema.column_list_.push_back(column_schema);
  }
  bool ok = storage_->CreateRelation(schema);
  if (ok) {
    message_ = ("relation " + schema.name_ + " created.");
  } else {
    message_ = ("relation " + schema.name_ + " create failed.");
  }
  return ok;
}

State QueryRealizer::ExecSelect() {
  output_->SendRowDescription(&output_row_desc_);
  bool ret = false;
  std::stringstream ssm;
  int rows = 0;
  BOOST_LOG_TRIVIAL(debug)<< "execute the plan";
  // execute the plan
  State state = top_exec_->Prepare();
  if (state != kStateOK) {
    return state;
  }
  state = top_exec_->Open();
  if (state != kStateOK) {
    return state;
  }

  TupleRow row(1);
  while (true) {
    state = top_exec_->GetNext(&row);
    if (state != kStateOK) {
      break;
    }
    rows++;
    output_->SendRowData(&row, &output_row_desc_);
  }
  top_exec_->Close();
  if (state == kStateEOF) {
    ssm << "SELECT " << rows << " rows." << std::endl;
    message_ = ssm.str();
    state = kStateOK;
  }
  return state;
}

QueryRealizerInterface* NewRealizer(RowOutputInterface *output,
                                    const std::string & sql) {
  return new QueryRealizer(output, sql);
}

void DeleteRealizer(QueryRealizerInterface* realizer) {
  delete realizer;
}
}  // end namespace realizer
