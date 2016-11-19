#ifndef CREATE_AS_STMT_H__
#define CREATE_AS_STMT_H__

#include <string>
#include <vector>
#include "parser/ast_base.h"
#include "parser/select_stmt.h"
namespace ast {
class CreateAsStmt : public ASTBase {
  friend ParserContext;
 public:

  CreateAsStmt(const std::string & table_name, SelectStmt *select_stmt)
      : table_name_(table_name),
        sub_select_(select_stmt),
        ASTBase(kASTCreateStmt) {
  }

  const std::string &TableName() const {
    return table_name_;
  }

  const std::vector<std::string> & OptColumnNameList() const {
    return opt_column_name_list_;
  }

  const SelectStmt *SubSelect() const {
    return sub_select_;
  }

  virtual ptree ToPropertyTree() const;
 private:
  CreateAsStmt();
  string table_name_;
  std::vector<std::string> opt_column_name_list_;
  SelectStmt * sub_select_;
};
}
#endif // CREATE_AS_STMT_H__
