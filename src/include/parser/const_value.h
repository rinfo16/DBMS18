#ifndef VALUE_H__
#define VALUE_H__

#include <string>
#include "parser/ast_base.h"
#include "parser/expression_base.h"

class ConstValue : public ExpressionBase {
 public:
  ConstValue(const std::string & str_val)
      : str_val_(str_val),
        int_val_(0),
        ExpressionBase(kASTConstValue) {
  }

  ConstValue(int64_t int_val)
      : int_val_(int_val),
        ExpressionBase(kASTConstValue) {
  }
  const std::string & StringValue() {
    return str_val_;
  }

  virtual ptree ToPropertyTree() const;

 private:
  ConstValue();
  std::string str_val_;
  int64_t int_val_;
};

#endif // VALUE_H__
