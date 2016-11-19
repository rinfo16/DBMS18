#include "query_realizer.h"
#include "parser/table_reference.h"
#include "parser/column_reference.h"
#include "parser/create_stmt.h"
#include "parser/join_clause.h"
#include "executor/seq_scan.h"
#include "executor/nested_loop_join.h"

namespace realizer {

QueryRealizer::QueryRealizer(RowOutputInterface *output,
                             const std::string & sql)
    : output_(output),
      parser_(NULL),
      parse_tree_(NULL),
      top_exec_(NULL) {
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
  State state;
  if (parse_tree_->ASTType() == kASTSelectStmt) {
    ast::SelectStmt *select_stmt = dynamic_cast<ast::SelectStmt *>(parse_tree_);
    state = CheckFrom(select_stmt);
    state = CheckWhere(select_stmt);
    state = CheckGroupBy(select_stmt);
    state = CheckSelect(select_stmt);
    state = CheckOrderBy(select_stmt);
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
    Relation *relation = storage_->GetMetaDataManager()->GetRelationByName(
        table_reference->TableName());
    if (relation == NULL) {
      return kStateTableNotFind;
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
        return kStateTableNotFind;
      }
      rel = iter->second;
      name.first = iter->first;
      slot_index = rel->GetAttributeIndex(ref->ColumnName());
      if (slot_index < 0) {
        return kStateAttributeNotFound;
      }
    } else {
      std::vector<uint32_t> attr_vec;
      for (auto iter = name2relation_.begin(); iter != name2relation_.end();
          ++iter) {
        Relation *rel = iter->second;
        int32_t i = rel->GetAttributeIndex(ref->ColumnName());
        if (i >= 0) {  // OK, we find the right attribute
          attr_vec.push_back(i);
          name.first = iter->first;
        }
      }
      if (attr_vec.size() > 1) {
        return kStateNameAmbiguous;
      } else if (attr_vec.empty()) {
        return kStateAttributeNotFound;
      }
      assert(attr_vec.size() == 1);
      slot_index = attr_vec[0];
    }
    assert(name2tuple_index_.find(name.first) != name2tuple_index_.end());
    executor::SlotReference slot_ref(name2tuple_index_[name.first], slot_index);
    if (ref->AliasName().empty()) {
      name.second = ref->AliasName();
    } else {
      name.second = ref->ColumnName();
    }
    name2slot_.insert(std::make_pair(name, slot_ref));
    if (type == kSelect) {
      select_ref_.push_back(ref);
    } else if (type == kGroupBy) {
      opt_group_by_ref_.push_back(ref);
    } else if (type == kOrderBy) {
      opt_order_by_ref_.push_back(ref);
    } else {
      assert(false);
    }
  } else if (kASTOperation) {
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
  } else {  // const value expression
  }
}

State QueryRealizer::CheckWhere(const ast::SelectStmt *select_stmt) {
  auto opt_where = select_stmt->OptWhere();
  for (size_t i = 0; i < opt_where.size(); i++) {
    CheckExpressionBase(opt_where[i], kWhere);
  }
}

State QueryRealizer::CheckGroupBy(const ast::SelectStmt *select_stmt) {
  auto opt_group_by = select_stmt->OptGroupBy();
  for (size_t i = 0; i < opt_group_by.size(); i++) {
    CheckExpressionBase(opt_group_by[i], kGroupBy);
  }
}

State QueryRealizer::CheckSelect(const ast::SelectStmt *select_stmt) {
  auto select_target = select_stmt->SelectList();
  for (auto i = 0; i < select_target.size(); i++) {
    const ast::SelectTarget *t = select_target[i];
    const ast::ExpressionBase *select_expr = t->SelectExpression();
    CheckExpressionBase(select_expr, kSelect);
  }
  return kStateOK;
}

State QueryRealizer::CheckOrderBy(const ast::SelectStmt *select_stmt) {
  auto opt_order_by = select_stmt->OptOrderBy();
  for (size_t i = 0; i < opt_order_by.size(); i++) {
    opt_order_by_is_desc_.push_back(opt_order_by[i]->IsDesc());
    CheckExpressionBase(opt_order_by[i]->OrderByExpression(), kOrderBy);
  }
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
    ast::CreateStmt *create_stmt = dynamic_cast<ast::CreateStmt *>(parse_tree_);
    ok = ExecCreate(create_stmt);
  } else if (parse_tree_->ASTType() == kASTCopyStmt) {
    ast::LoadStmt *load_stmt = dynamic_cast<ast::LoadStmt *>(parse_tree_);
    ok = ExecLoad(load_stmt);
  } else if (parse_tree_->ASTType() == kASTSelectStmt) {
    ast::SelectStmt *select_stmt = dynamic_cast<ast::SelectStmt *>(parse_tree_);
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

bool QueryRealizer::BuildSelect(ast::SelectStmt *select_stmt) {
  bool ret = false;
  storage::IteratorInterface *iter = NULL;
  std::string table_name;
  auto table_factor_list = select_stmt->TableReference();
  const ast::TableFactor * table_factor = table_factor_list[0];
  if (table_factor->ASTType() == kASTTableReference) {
    const ast::TableReference *table_reference =
        dynamic_cast<const ast::TableReference*>(table_factor);
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
    const ast::ExpressionBase *expr = select_list[i]->SelectExpression();
    if (expr->ASTType() == kASTColumnReference) {
      const ast::ColumnReference *column_reference =
          dynamic_cast<const ast::ColumnReference*>(expr);
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
  return true;
}

bool QueryRealizer::BuildJoin(
    const std::vector<ast::TableFactor *> table_factor_list) {
  for (auto i = 0; i < table_factor_list.size(); i++) {

  }
  return true;
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
      return NULL;
    }
    const ast::Operation *operation =
        static_cast<const ast::Operation*>(join_clause->JoinPredicate());
    executor::BooleanExprInterface *boolean_expr = BuildBooleanExpression(
        operation);
    executor::ExecInterface* join_exec = new executor::NestedLoopJoin(
        left_exec, right_exec, boolean_expr);
    all_exec_obj_.push_back(join_exec);
    return join_exec;
  }
  assert(false);
  return NULL;
}

executor::BooleanExprInterface *QueryRealizer::BuildBooleanExpression(
    const ast::Operation *operation) {

}
executor::ExecInterface* QueryRealizer::BuildTableScan(
    const ast::TableReference *table_reference) {
  std::string table_name = table_reference->TableName();
  storage::IteratorInterface *iter = storage_->NewIterator(table_name);
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
}

bool QueryRealizer::ExecLoad(ast::LoadStmt *load_stmt) {
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

QueryRealizerInterface* NewRealizer(RowOutputInterface *output,
                                    const std::string & sql) {
  return new QueryRealizer(output, sql);
}

void DeleteRealizer(QueryRealizerInterface* realizer) {
  delete realizer;
}
}  // end namespace realizer
