#ifndef __CREATE_TABLE_STMT_H__
#define __CREATE_TABLE_STMT_H__

#include <string>
#include <vector>
#include <assert.h>
#include "common/data_type.h"
#include "parser/ast_base.h"
#include "parser/select_stmt.h"
#include "reference_name.h"

using std::string;
namespace ast {
class ColumnDefine : public ASTBase {
 public:
  ColumnDefine(const std::string & column_name, DataType column_type, int32_t data_length)
      : column_name_(column_name),
        data_type_(column_type),
        data_length_(data_length),
        ASTBase(kASTColumnDefine),
        is_primary_key_(false){

  }

    ColumnDefine(const std::string & column_name, DataType column_type, int32_t data_length, bool primary_key)
            : column_name_(column_name),
              data_type_(column_type),
              data_length_(data_length),
              ASTBase(kASTColumnDefine),
              is_primary_key_(primary_key){

    }
  DataType ColumnType() const {
    return data_type_;
  }

  const std::string & ColumnName() const {
    return column_name_;
  }

  int32_t DataLength() const {
    return data_length_;
  }

  virtual ptree ToPropertyTree() const;

  bool IsPrimaryKey() { return is_primary_key_;}
 private:
  ColumnDefine();
  string column_name_;
  DataType data_type_;
  int32_t data_length_;
  bool is_primary_key_;
 private:
};

class CreateStmt : public ASTBase {
  friend ParserContext;
 public:

  CreateStmt()
      : ASTBase(kASTCreateStmt) {
  }

  std::string TableName() {
    return table_name_;
  }

  std::string PrimaryKey() {
    return primary_key_;
  }

  const std::vector<ColumnDefine*> & ColumnDefineList() const {
    return column_define_list_;
  }

  virtual ptree ToPropertyTree() const;
 private:
  string table_name_;
  string primary_key_;
  std::vector<ColumnDefine*> column_define_list_;
};
}
#endif // __CREATE_TABLE_STMT_H__
