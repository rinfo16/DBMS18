#ifndef __NAME_H__
#define __NAME_H__

#include <string>

#include "parser/ast_base.h"

using std::string;

class name : public ASTBase {
 public:

  string str_val_;

  TRACE_FUNC_DECLARE;
};

#endif //__NAME_H__
