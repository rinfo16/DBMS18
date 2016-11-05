#ifndef SRC_INCLUDE_PARSER_SQL_PARSER_H_
#define SRC_INCLUDE_PARSER_SQL_PARSER_H_

#include "parser/sql_parser_interface.h"
#include "parser/ast_base.h"
#include "parser/parser_context.h"

namespace parser {

class SQLParser : public SQLParserInterface {
 public:
  SQLParser(const std::string & sql);
  virtual ~SQLParser();
  ASTBase* Parse();
 private:
  std::string sql_;
  ParserContext *parser_context_;
};

}  // namespace Parser

#endif // SRC_INCLUDE_PARSER_SQL_PARSER_H_
