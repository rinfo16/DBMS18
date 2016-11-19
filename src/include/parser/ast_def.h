#ifndef __AST_DEF_H__
#define __AST_DEF_H__

#include "common/data_type.h"

#define	ORD_ASC 1
#define	ORD_DESC 2

#define JOIN_NONE 0
#define JOIN_INNER 1
#define JOIN_LEFT 2
#define JOIN_RIGHT 3
#define JOIN_CROSS 4
#define JOIN_STRAIGHT 5
#define JOIN_NATURAL_INNER 6
#define JOIN_NATURAL_LEFT 7
#define JOIN_NATURAL_RIGHT 8

#define TABLE_SCOPE_TMP 1

#define DATA_TYPE_INTEGER 1
#define DATA_TYPE_FLOAT 2

typedef int order_t;
typedef int join_type_t;
typedef int table_scope_t;
typedef int data_type_t;

enum OperatorType {
  kInvalid,
  kAssign,
  kEqual,
  kLess,
  kGreater,
  kLessOrEqual,
  kGreaterOrEaual,
};

enum OrderType {
  kAsc,
  kDesc
};

enum JoinType {
  kJoinLeft,
  kJoinRight,
  kJoinInner,
  kJoinOuter
};

enum TreeType {
  kASTBase,
  kASTSelectStmt,
  kASTCreateStmt,
  kASTCreateAsStmt,
  kASTUpdateStmt,
  kASTInsertStmt,
  kASTCopyStmt,
  kASTOperation,
  kASTSelectTarget,
  kASTJoinClause,
  kASTOrderClause,
  kASTReferenceName,
  kASTTableFactor,
  kASTColumnReference,
  kASTColumnDefine,
  kASTConstValue,
  kASTTableReference,
  kASTSubQuery
};


#endif // __AST_DEF_H__
