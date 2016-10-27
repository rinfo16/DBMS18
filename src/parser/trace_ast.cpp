#include "../include/parser/trace_ast.h"

#include <iostream>
#include "parser/ast_base.h"
#include "create_stmt.h"
#include "insert_stmt.h"
#include "join_table.h"
#include "name.h"
#include "select_stmt.h"
#include "table_factor.h"

#ifdef TRACE_AST

void ASTBase::trace(int depth) {
  for (int i = 0; i < depth; i++) {
    std::cout << "--";
  }
}

void name::trace(int depth) {
  depth++;
  ASTBase::trace(depth);
  std::cout << "NAME : " << str_val_ << std::endl;
}

void OrderBy::trace(int depth) {
  depth++;
  ASTBase::trace(depth);
  std::cout << " ORD :  " << order_ << " ";
  expr_->trace(depth);
}

void SelectStmt::trace(int depth) {
  depth++;
  ASTBase::trace(depth);
  std::cout << "SELECT STMT : " << std::endl;

  depth++;
  if (select_list_) {
    ASTBase::trace(depth);
    std::cout << "SELECT LIST : " << std::endl;
    ASTBase *p = select_list_;
    while (p != NULL) {
      p->trace(depth);
      p = p->next_;
    }
  }

  if (table_reference_) {
    ASTBase::trace(depth);
    std::cout << "SELECT TABLE LIST : " << std::endl;
    ASTBase *p = table_reference_;
    while (p != NULL) {
      p->trace(depth);
      p = p->next_;
    }
  }

  if (opt_where_) {
    std::cout << "OPT WHERE : " << std::endl;
    ASTBase *p = opt_where_;
    while (p != NULL) {
      p->trace(depth);
      p = p->next_;
    }
  }

  if (opt_group_) {
    ASTBase::trace(depth);
    std::cout << "OPT GROUP BY: " << std::endl;
    ASTBase *p = opt_group_;
    while (p != NULL) {
      p->trace(depth);
      p = p->next_;
    }
  }

  if (opt_have_) {
    ASTBase::trace(depth);
    std::cout << "OPT HAVING: " << std::endl;
    ASTBase *p = opt_have_;
    while (p != NULL) {
      p->trace(depth);
      p = p->next_;
    }
  }

  if (opt_order_) {
    ASTBase::trace(depth);
    std::cout << "OPT ORDER BY: " << std::endl;
    ASTBase *p = opt_order_;
    while (p != NULL) {
      p->trace(depth);
      p = p->next_;
    }
  }
}

void TableFactor::trace(int depth) {
  depth++;
  ASTBase::trace(depth);
  std::cout << "TABLE : " << table_name_ << " ALIAS : " << alias_name_
            << std::endl;

  if (sub_query_) {
    ASTBase::trace(depth);
    std::cout << "SUBQUERY : ";
    sub_query_->trace(depth);
  }
}

void JoinTable::trace(int depth) {
  depth++;
  ASTBase::trace(depth);
  std::cout << "JOIN,  TYPE : " << join_type_ << std::endl;
  if (left_table_) {
    ASTBase::trace(depth);
    std::cout << "LEFT TABLE : ";
    left_table_->trace(depth);
  }
  if (right_table_) {
    ASTBase::trace(depth);
    std::cout << "RIGHT TABLE : ";
    right_table_->trace(depth);
  }
}

void InsertStmt::trace(int depth) {
  depth++;
  ASTBase::trace(depth);
  std::cout << "INSERT STMT" << table_name_ << std::endl;

  ASTBase * t = opt_column_names_;

  while (t != NULL) {
    t->trace(depth);
    t = t->next_;
  }

  t = insert_values_;
  while (t != NULL) {
    t->trace(depth);
    t = t->next_;
  }
}

void CreateStmt::trace(int depth) {
  depth++;
  ASTBase::trace(depth);

  std::cout << "CRAEATE TABLE STMT " << " TABLE_NAME : " << table_name_
            << " SCOPE " << table_scope_ << std::endl;

  ASTBase * t = column_define_list_;
  while (t != NULL) {
    t->trace(depth);
    t = t->next_;
  }

  t = opt_column_name_list_;
  while (t != NULL) {
    t->trace(depth);
    t = t->next_;
  }

  if (subquery_)
    subquery_->trace(depth);

}

void column_def::trace(int depth) {
  depth++;
  ASTBase::trace(depth);

  std::cout << "COLUMN DEFINITION : " << column_name_ << " " << data_type_
            << " " << std::endl;

}
#endif
