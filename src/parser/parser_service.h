#ifndef PARSER_SERVICE_H__
#define PARSER_SERVICE_H__

#include "parser/parser_service_interface.h"
namespace parser {
class ParserService : public ParserServiceInterface {
public:
  ParserService();
  virtual ~ParserService();
  virtual SQLParserInterface *CreateSQLParser(const std::string sql);
  virtual void DestroySQLParser(SQLParserInterface *);
};
} // end namespace parser

#endif // PARSER_SERVICE_H__
