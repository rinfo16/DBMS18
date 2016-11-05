#ifndef __AST_BASE_H__
#define __AST_BASE_H__

#include <vector>
#include <iostream>
#include <cstddef>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "ast_def.h"
class ParserContext;
using boost::property_tree::ptree;
class ASTBase {
  friend ParserContext;
 public:
  ASTBase()
      : prev_(NULL),
        next_(NULL),
        tree_type_(kASTBase) {
  }

  ASTBase(TreeType type)
      : prev_(NULL),
        next_(NULL),
        tree_type_(type) {
  }

  void RAppend(ASTBase *ast) {
    ASTBase *last = this;
    while (last->next_) {
      last = last->next_;
    }
    ast->prev_ = last;
    last->next_ = ast;
  }

  void LAppend(ASTBase *ast) {
    ASTBase *first = this;
    while (first->prev_) {
      first = first->prev_;
    }
    ast->next_ = first;
    first->prev_ = ast;
  }

  virtual ptree ToPropertyTree() const = 0;

  std::string ToJSON() const {
    std::stringstream ssm;
    ptree tree = this->ToPropertyTree();
    boost::property_tree::write_json(ssm, tree);
    return ssm.str();
  }

  ASTBase *Prev() {
    return prev_;
  }

  ASTBase *Next() {
    return next_;
  }

  virtual ~ASTBase() {
  }

  TreeType ASTType() const {
    return tree_type_;
  }

 private:
  ASTBase *prev_;
  ASTBase *next_;
  TreeType tree_type_;
};

#endif
