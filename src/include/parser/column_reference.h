#ifndef COLUMN_REFERENCE_H__
#define COLUMN_REFERENCE_H__

#include "parser/expression_base.h"
namespace ast {
class ColumnReference : public ExpressionBase {
 public:
  ColumnReference(const std::string & column_name)
      : column_name_(column_name),
        ExpressionBase(kASTColumnReference) {
  }

  ColumnReference(const std::string & table_name,
                  const std::string & column_name)
      : table_name_(table_name),
        column_name_(column_name),
        ExpressionBase(kASTColumnReference) {
  }

  ColumnReference()
      : ExpressionBase(kASTColumnReference) {
  }

  void SetTableName(const std::string & table_name) {
    table_name_ = table_name;
  }

  void SetAliasName(const std::string & alias_name) {
    alias_name_ = alias_name;
  }

  const std::string & TableName() const {
    return table_name_;
  }
  const std::string & ColumnName() const {
    return column_name_;
  }

  const std::string & AliasName() const {
    return alias_name_;
  }

  virtual ptree ToPropertyTree() const;

 private:
  std::string table_name_;
  std::string column_name_;
  std::string alias_name_;
};
}
#endif // COLUMN_REFERENCE_H__
