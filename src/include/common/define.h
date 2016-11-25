#ifndef DEFINE_H_
#define DEFINE_H_

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include "common/boost_log.h"

#define PAGE_SIZE                 8192
#define PAGE_NUMBER_PER_EXTENT    320
#define MAX_PAGE_NUMBER_PER_FILE  8192000
#define MAX_EXTENT_COUNT          40000
#define BUFFER_POOL_SIZE          819200
#define DATA_DIRECTORY            "/tmp"
#define FLIP_NUMBER               20161012
#define SEGMENT_DESCRIPT_FILE_NO  0
#define DATA_FILE_NO              1
#define FILE_HEADER_PAGE_NO       0
#define DEFAULT_PORT 8432

#define STR_NAME                  "NAME"
#define STR_ID                    "ID"
#define STR_RELATION_ID           "RELATION_ID"
#define STR_MAX_LENGTH            "MAX_LENGTH"
#define STR_DATA_TYPE             "DATA_TYPE"
#define STR_DATA_TYPE_LENGTH      "DATA_TYPE_LENGTH"
#define STR_IS_NULL               "IS_NULL"
#define STR_IS_VARIABLE           "IS_VARIABLE"
#define STR_FIRST_DATA_PAGE       "FIRST_DATA_PAGE"
#define STR_LAST_DATA_PAGE        "LAST_DATA_PAGE"
#define STR_FIRST_EXTENT          "FIRST_EXTENT"
#define STR_LAST_EXTENT           "LAST_EXTENT"
#define STR_FILE_NO               "FILE_NO"
#define STR_PAGE_NO               "PAGE_NO"
#define STR_ATTRIBUTE_LIST        "ATTRIBUTE_LIST"
#define STR_DATA_TYPE "DATA_TYPE"
#define STR_IS_DESC "IS_DESC"
#define STR_JOIN_TYPE "JOIN_TYPE"
#define STR_OPERATOR_TYPE "OPERATOR_TYPE"
#define STR_LEFT "LEFT"
#define STR_RIGHT "RIGHT"
#define STR_COLUMN_NAME "COLUMN_NAME"
#define STR_TABLE_NAME "TABLE_NAME"
#define STR_ALIAS_NAME "ALIAS_NAME"
#define STR_FILE_PATH "FILE_PATH"
#define STR_CONST_VALUE "VALUE"
#define STR_COLUMN_NAME_LIST "COLUMN_NAME_LIST"
#define STR_SUB_SELECT "SUB_SELECT"
#define STR_COLUMN_DEFINE "COLUMN_DEFINE"
#define STR_COLUMN_DEFINE_LIST "COLUMN_DEFINE_LIST"
#define STR_SELECT_EXPRESSION "SELECT_EXPRESSION"
#define STR_WHERE_EXPRESSION "WHERE_EXPRESSION"
#define STR_GROUP_BY_EXPRESSION "GROUP_BY_EXPRESSION"
#define STR_ORDER_BY_EXPRESSION "ORDER_BY_EXPRESSION"
#define STR_SELECT_LIST "SELECT_LIST"
#define STR_TABLE_FACTOR_LIST "TABLE_FACTOR_LIST"
#define STR_WHERE_LIST "WHERE_LIST"
#define STR_GROUP_BY_LIST "GROUP_BY_LIST"
#define STR_ORDER_BY_LIST "ORDER_BY_LIST"


#define LOW_INT16(l)              ((int16_t)((int32_t)(l) & 0xffff))
#define HIGH_INT16(l)             ((int16_t)((int32_t)(l) >> 16))
#define MAKE_INT32(high16, low16) ((int32_t) high16 << 16 | (int32_t) low16)

typedef uint32_t fileno_t;
typedef uint32_t segmentno_t;
typedef uint32_t extentno_t;
typedef uint32_t pageno_t;
typedef uint32_t slotno_t;
typedef uint32_t relationid_t;
typedef uint32_t attributeid_t;
typedef double double_t;


typedef int order_t;
typedef int join_type_t;
typedef int table_scope_t;
typedef int data_type_t;

enum OperatorType {
  kInvalid,
  kAssign,

  kBooleanBegin,

  kCompareBegin,
  kEqual,
  kLess,
  kGreater,
  kLessOrEqual,
  kGreaterOrEaual,
  kCompareEnd,

  kLogicBegin,
  kAnd,
  kOr,
  kXor,
  kNot,
  kLogicEnd,

  kBooleanEnd,

  kArithmeticBegin,
  kArithmeticEnd,
};

enum OrderType {
  kAsc,
  kDesc
};

enum JoinType {
  kJoinLeftOutter,
  kJoinRightOutter,
  kJoinFullOutter,
  kJoinInner,
  kJoinCross
};

enum TreeType {
  kASTBase,
  kASTSelectStmt,
  kASTCreateStmt,
  kASTCreateAsStmt,
  kASTUpdateStmt,
  kASTInsertStmt,
  kASTCopyFromStmt,
  kASTCopyToStmt,
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

enum FromOrTo {
  kFrom,
  kTo
};

bool IsArithmeticOperator(OperatorType);
bool IsBooleanOperator(OperatorType);
bool IsCompareOperator(OperatorType);
bool IsLogicOperator(OperatorType);
#endif // DEFINE_H_
