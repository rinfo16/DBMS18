#ifndef __AST_DEF_H__
#define __AST_DEF_H__


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

#endif // __AST_DEF_H__
