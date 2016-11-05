#ifndef SQL_PARSER_INTERFACE_H__
#define SQL_PARSER_INTERFACE_H__

#include "parser/ast_base.h"

namespace parser {
class SQLParserInterface {
 public:
  virtual ASTBase* Parse() = 0;
  virtual ~SQLParserInterface(){}
};
}
#endif // SQL_PARSER_INTERFACE_H__
