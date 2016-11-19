
#ifndef __INSERT_STMT_H__
#define __INSERT_STMT_H__

#include <string>
#include "parser/ast_base.h"
#include "parser/const_value.h"
namespace ast {
class InsertStmt : public ASTBase {
  friend ParserContext;
 public:
  InsertStmt()
      : ASTBase(kASTInsertStmt){
  }

  const std::string & TableName() const {
    return table_name_;
  }

  std::vector<std::string> & OptColumnNameList() {
    return opt_column_names_;
  }

  const std::vector<std::string> & OptColumnNameList() const {
    return opt_column_names_;
  }

  std::vector<ConstValue*> & ValueList() {
    return insert_values_;
  }

  const std::vector<ConstValue*> & ValueList() const {
    return insert_values_;
  }

  virtual ptree ToPropertyTree() const;
 private:
  std::string table_name_;
  std::vector<std::string> opt_column_names_;
  std::vector<ConstValue*> insert_values_;
};
}
#endif //__INSERT_STMT_H__
