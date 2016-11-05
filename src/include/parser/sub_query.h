#ifndef SUB_QUERY_H__
#define SUB_QUERY_H__

#include "parser/table_factor.h"

class SelectStmt;

class SubQuery : public TableFactor {
 public:
  SubQuery(SelectStmt *stmt)
      : select_stmt_(stmt),
        TableFactor(kASTSubQuery) {
  }

  SubQuery(SelectStmt *stmt, std::string alias)
      : select_stmt_(stmt),
        alias_name_(alias),
        TableFactor(kASTSubQuery) {
  }
  const SelectStmt *SubSelectStmt() const {
    return select_stmt_;
  }

  SelectStmt *SubSelectStmt() {
    return select_stmt_;
  }

  const std::string & AliasName() const {
    return alias_name_;
  }

  virtual ptree ToPropertyTree() const;
 private:
  SubQuery();
  SelectStmt *select_stmt_;
  std::string alias_name_;
};

#endif // SUB_QUERY_H__
