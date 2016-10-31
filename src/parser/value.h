#ifndef VALUE_H__
#define VALUE_H__

#include <string>

#include "parser/ast_base.h"

using std::string;

class Value : public ASTBase {
 public:

  string str_val_;

  TRACE_FUNC_DECLARE;
};

#endif // VALUE_H__
