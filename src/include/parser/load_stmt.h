#ifndef LOAD_STMT_H__
#define LOAD_STMT_H__
#include "parser/ast_base.h"
#include <string>
#include <vector>
namespace ast {

class LoadStmt : public ASTBase {
  friend ParserContext;
 public:
  LoadStmt() : from_stdin_(false), ASTBase(kASTCopyFromStmt) {
  }
  const std::string & TableName() const {
    return table_name_;
  }
  const std::string & FilePath() const {
    return file_path_;
  }
  const std::vector<std::string> & OptColumnNameList() const {
    return opt_column_name_list_;
  }
  virtual ptree ToPropertyTree() const;

  bool IsFromStdin() const {
    return from_stdin_;
  }

  int Delimiter() const { return copy_delimiter_; }
 private:
  bool from_stdin_;
  std::string table_name_;
  std::string file_path_;
  int copy_delimiter_;
  std::vector<std::string> opt_column_name_list_;
};
}
#endif // LOAD_STMT_H__
