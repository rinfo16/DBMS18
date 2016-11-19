#ifndef __NAME_H__
#define __NAME_H__

#include <string>

#include "parser/ast_base.h"

using std::string;
namespace ast {
class ReferenceName : public ASTBase {
 public:
  ReferenceName(const string & name)
      : str_val_(name),
        ASTBase(kASTReferenceName) {
  }
  const std::string & String() const {
    return str_val_;
  }
  virtual ptree ToPropertyTree() const;
 private:
  string str_val_;
};
}
#endif //__NAME_H__
