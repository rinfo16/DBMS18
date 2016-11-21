#ifndef SQL_PARSER_INTERFACE_H__
#define SQL_PARSER_INTERFACE_H__

#include "parser/ast_base.h"
#include <string>
namespace parser {
class SQLParserInterface {
 public:
  virtual ast::ASTBase* Parse(std::string & output_error_message) = 0;
  virtual ~SQLParserInterface(){}
};
}
#endif // SQL_PARSER_INTERFACE_H__
