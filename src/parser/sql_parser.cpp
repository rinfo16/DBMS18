#include "sql_parser.h"

namespace parser {

SQLParser::SQLParser(const std::string & sql)
    : sql_(sql),
      parser_context_(NULL) {

}

SQLParser::~SQLParser() {
  delete parser_context_;
}

ASTBase* SQLParser::Parse() {
  parser_context_ = new ParserContext(sql_);
  if (parser_context_->Parse()) {
    return parser_context_->GetAST();
  }
  return NULL;
}

}  // namespace Parser
