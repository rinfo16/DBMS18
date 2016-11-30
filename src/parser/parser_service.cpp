#include "parser_service.h"
#include "sql_parser.h"
#include "common/singleton.hpp"
//#include "parser/parser_service_interface.h"

namespace parser {

ParserService::ParserService() {
  // TODO Auto-generated constructor stub

}

ParserService::~ParserService() {
  // TODO Auto-generated destructor stub
}

SQLParserInterface *ParserService::CreateSQLParser(const std::string sql) {
  return new SQLParser(sql);
}

void ParserService::DestroySQLParser(SQLParserInterface * sql_parser) {
  delete sql_parser;
}

ParserServiceInterface *ParserServiceInterface::Instance() {
  return & utils::Singleton<ParserService>::instance();
}

}
// namespace Parser
