#ifndef SRC_PARSER_PARSER_SERVICE_INTERFACE_H_
#define SRC_PARSER_PARSER_SERVICE_INTERFACE_H_


#include "parser/sql_parser_interface.h"
namespace parser {

class ParserServiceInterface {
 public:
  virtual ~ParserServiceInterface() {};
  virtual SQLParserInterface *CreateSQLParser(const std::string sql) = 0;
  virtual void DestroySQLParser(SQLParserInterface *) = 0;
  static ParserServiceInterface *Instance();
};

}  // namespace Parser

#endif // SRC_PARSER_PARSER_SERVICE_INTERFACE_H_
