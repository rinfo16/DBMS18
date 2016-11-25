#ifndef EXPORT_STMT_H_
#define EXPORT_STMT_H_

#include "parser/ast_base.h"

namespace ast {

class SelectStmt;

class ExportStmt : public ASTBase {
  friend ParserContext;
 public:
  ExportStmt()
      : to_stdout_(false),
        ASTBase(kASTCopyToStmt) {
  }

  ~ExportStmt() {
  }
  const std::string & FilePath() const {
    return file_path_;
  }
  const std::string & TableName() const {
    return table_name_;
  }
  const std::vector<std::string> & OptColumnNameList() const {
    return opt_column_name_list_;
  }
  bool IsToStdout() const {
    return to_stdout_;
  }
  virtual ptree ToPropertyTree() const;
 private:
  bool to_stdout_;
  std::string table_name_;
  std::string file_path_;
  std::vector<std::string> opt_column_name_list_;
};

}  // namespace ast

#endif // EXPORT_STMT_H_
