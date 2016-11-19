#ifndef VALUE_H__
#define VALUE_H__

#include <string>
#include "parser/ast_base.h"
#include "parser/expression_base.h"
#include "common/data_type.h"

namespace ast {
class ConstValue : public ExpressionBase {
 public:
  ConstValue(const std::string & str_val)
      : str_val_(str_val),
        int_val_(0),
        data_type_(kDTVarchar),
        ExpressionBase(kASTConstValue) {
  }

  ConstValue(int64_t int_val)
      : int_val_(int_val),
        data_type_(kDTInteger),
        ExpressionBase(kASTConstValue) {
  }

  DataType GetDataType() const {
    return data_type_;
  }
  const std::string & GetStringValue() const{
    return str_val_;
  }

  int64_t GetIntegerValue() const{
    return int_val_;
  }
  virtual ptree ToPropertyTree() const;

 private:
  ConstValue();
  DataType data_type_;
  std::string str_val_;
  int64_t int_val_;
};
}
#endif // VALUE_H__
