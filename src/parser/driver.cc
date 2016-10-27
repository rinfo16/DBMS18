// $Id$
/** \file driver.cc Implementation of the parser::Driver class. */

#include <fstream>
#include <sstream>

#include "driver.h"

#include "parser_context.h"
#include "scanner.h"

namespace parser {

Driver::Driver()
    : trace_scanning(false),
      trace_parsing(false),
      lexer(NULL) {
}

bool Driver::parse_stream(ParserContext & ctx, std::istream& in,
                          const std::string& sname) {
  streamname = sname;

  Scanner scanner(&in);
  scanner.set_debug(trace_scanning);
  this->lexer = &scanner;

  Parser par(ctx);
  par.set_debug_level(trace_parsing);
  return (par.parse() == 0);
}

bool Driver::parse_file(ParserContext & ctx, const std::string &filename) {
  std::ifstream in(filename.c_str());
  if (!in.good())
    return false;
  return parse_stream(ctx, in, filename);
}

bool Driver::parse_string(ParserContext & ctx, const std::string &input,
                          const std::string& sname) {
  std::istringstream iss(input);
  return parse_stream(ctx, iss, sname);
}

void Driver::error(const class location& l, const std::string& m) {
  std::cerr << l << ": " << m << std::endl;
}

void Driver::error(const std::string& m) {
  std::cerr << m << std::endl;
}

}  // namespace parser
