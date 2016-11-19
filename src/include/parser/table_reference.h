#ifndef TABLE_REFERENCE_H__
#define TABLE_REFERENCE_H__

#include "parser/table_factor.h"
namespace ast {
class TableReference : public TableFactor {
 public:
  TableReference(const std::string & table_name)
      : TableFactor(kASTTableReference) {
  }

  void SetAliasName(const std::string & alias_name) {
    table_name_ = alias_name;
  }
  const std::string & TableName() const {
    return table_name_;
  }
  const std::string & AliasName() const {
    return alias_name_;
  }
  virtual ptree ToPropertyTree() const;
 private:
  std::string table_name_;
  std::string alias_name_;
};
}
#endif // TABLE_REFERENCE_H__
