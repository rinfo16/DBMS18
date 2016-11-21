#include "sql_parser.h"

namespace parser {

SQLParser::SQLParser(const std::string & sql)
    : sql_(sql),
      parser_context_(NULL) {

}

SQLParser::~SQLParser() {
  delete parser_context_;
}

ASTBase* SQLParser::Parse(std::string & output_error_message) {
  parser_context_ = new ParserContext(sql_);
  if (parser_context_->Parse()) {
    return parser_context_->GetAST();
  }
  output_error_message = parser_context_->ErrorMessage();
  return NULL;
}

}  // namespace Parser
