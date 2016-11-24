/* $Id$ -*- mode: c++ -*- */
/** \file parser.yy Contains the parser Bison parser source */

%{ /*** C/C++ Declarations ***/

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include "common/define.h"
#include "parser/ast_base.h"
#include "parser_context.h"

#define yyerror(s) (ctx.driver_.error(s))
#define YYDEBUG 1


%}

/*** yacc/bison Declarations ***/

/* Require bison 2.1 or later */
%require "2.5"

/* add debug output code to generated parser. disable this for release
 * versions. */
%debug


/* write out a header file containing the token defines */
%defines

/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
%name-prefix="parser"


/* set the parser's class identifier */
%define "parser_class_name" "Parser"

/* keep track of the current position within the input */
%locations
%initial-action
{
  // initialize the initial location object
  @$.begin.filename = @$.end.filename = &ctx.driver_.streamname;
};

/* The driver is passed by reference to the parser and to the scanner. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class ParserContext & ctx }

/* verbose error messages */
%error-verbose


 /*** BEGIN PARSER - Change the parser grammar's tokens below ***/
%union 
{
  int64_t intval;
  double floatval;
  char *strval;
  int subtok;
  class select_stmt *selstmt_ptr;
  class name *name_ptr;
  class ASTBase *astbase_ptr;
}

/* names and literal values */
%token <strval> NAME
%token <strval> STRING
%token <intval> INTNUM
%token <intval> BOOL
%token <floatval> APPROXNUM

/* user @abc names */
%token <strval> USERVAR

/* operators and precedence levels */
%right ASSIGN
%left OR
%left XOR
%left ANDOP
%nonassoc IN IS LIKE REGEXP
%left NOT '!'
%left BETWEEN
%left <subtok> COMPARISON /* = <> < > <= >= <=> */
%left '|'
%left '&'
%left <subtok> SHIFT /* << >> */
%left '+' '-'
%left '*' '/' '%' MOD
%left '^'
%nonassoc UMINUS

/* tokens */
%token SQLEND 0 "sql end"
%token ADD
%token ALL
%token ALTER
%token ANALYZE
%token AND
%token ANY
%token AS
%token ASC
%token AUTO_INCREMENT
%token BEFORE
%token BIGINT
%token BINARY
%token BIT
%token BLOB
%token BOTH
%token BY
%token CALL
%token CASCADE
%token CASE
%token CHANGE
%token CHAR
%token CHECK
%token COLLATE
%token COLUMN
%token COMMA
%token COMMENT
%token CONDITION
%token CONSTRAINT
%token CONTINUE
%token CONVERT
%token COPY
%token CREATE
%token CROSS
%token CSV
%token CURRENT_DATE
%token CURRENT_TIME
%token CURRENT_TIMESTAMP
%token CURRENT_USER
%token CURSOR
%token DATABASE
%token DATABASES
%token DATE
%token DATETIME
%token DAY_HOUR
%token DAY_MICROSECOND
%token DAY_MINUTE
%token DAY_SECOND
%token DECIMAL
%token DECLARE
%token DEFAULT
%token DELAYED
%token DELETE
%token DELIMITER
%token DESC
%token DESCRIBE
%token DETERMINISTIC
%token DISTINCT
%token DISTINCTROW
%token DIV
%token DOUBLE
%token DROP
%token DUAL
%token EACH
%token ELSE
%token ELSEIF
%token ENCLOSED
%token END
%token ENUM
%token EQUAL
%token ESCAPED
%token <subtok> EXISTS
%token EXIT
%token EXPLAIN
%token FETCH
%token FLOAT
%token FOR
%token FORCE
%token FOREIGN
%token FROM
%token FULLTEXT
%token GRANT
%token GROUP
%token HAVING
%token HIGH_PRIORITY
%token HOUR_MICROSECOND
%token HOUR_MINUTE
%token HOUR_SECOND
%token IF
%token IGNORE
%token INDEX
%token INFILE
%token INNER
%token INOUT
%token INSENSITIVE
%token INSERT
%token INT
%token INTEGER
%token INTERVAL
%token INTO
%token ITERATE
%token JOIN
%token KEY
%token KEYS
%token KILL
%token LEADING
%token LEAVE
%token LEFT
%token LIMIT
%token LINES
%token LOAD
%token LOCALTIME
%token LOCALTIMESTAMP
%token LOCK
%token LONG
%token LONGBLOB
%token LONGTEXT
%token LOOP
%token LOW_PRIORITY
%token MATCH
%token MEDIUMBLOB
%token MEDIUMINT
%token MEDIUMTEXT
%token MINUTE_MICROSECOND
%token MINUTE_SECOND
%token MODIFIES
%token NATURAL
%token NO_WRITE_TO_BINLOG
%token NULLX
%token NUMBER
%token ON
%token ONDUPLICATE
%token OPERATER
%token OPTIMIZE
%token OPTION
%token OPTIONALLY
%token ORDER
%token OUT
%token OUTER
%token OUTFILE
%token PAREN_LEFT
%token PAREN_RIGHT
%token PRECISION
%token PRIMARY
%token PROCEDURE
%token PURGE
%token QUICK
%token QUOTE
%token READ
%token READS
%token REAL
%token REFERENCES
%token RELEASE
%token RENAME
%token REPEAT
%token REPLACE
%token REQUIRE
%token RESTRICT
%token RETURN
%token REVOKE
%token RIGHT
%token POINT
%token ROLLUP
%token SCHEMA
%token SCHEMAS
%token SECOND_MICROSECOND
%token SELECT
%token SEMICOLON
%token SENSITIVE
%token SEPARATOR
%token SET
%token SHOW
%token SMALLINT
%token SOME
%token SONAME
%token SPATIAL
%token SPECIFIC
%token SQL
%token SQLEXCEPTION
%token SQLSTATE
%token SQLWARNING
%token SQL_BIG_RESULT
%token SQL_CALC_FOUND_ROWS
%token SQL_SMALL_RESULT
%token SSL
%token STARTING
%token STDIN
%token STDOUT
%token STRAIGHT_JOIN
%token TABLE
%token TEMPORARY
%token TEXT
%token TERMINATED
%token THEN
%token TIME
%token TIMESTAMP
%token TINYBLOB
%token TINYINT
%token TINYTEXT
%token TO
%token TRAILING
%token TRIGGER
%token UNDO
%token UNION
%token UNIQUE
%token UNLOCK
%token UNSIGNED
%token UPDATE
%token USAGE
%token USE
%token USING
%token UTC_DATE
%token UTC_TIME
%token UTC_TIMESTAMP
%token VALUES
%token VARBINARY
%token VARCHAR
%token VARYING
%token WHEN
%token WHERE
%token WHILE
%token WITH
%token WRITE
%token YEAR
%token YEAR_MONTH
%token ZEROFILL
%token FSUBSTRING
%token FTRIM
%token FDATE_ADD
%token FDATE_SUB
%token FCOUNT

%type <intval> select_opts 
%type <intval> val_list opt_val_list case_list
%type <intval> opt_asc_desc
%type <intval> opt_outer
%type <intval> opt_join_type
%type <intval> index_list opt_for_join
%type <intval> column_atts data_type 
%type <intval> from_or_to

%type <astbase_ptr> table_name column_name opt_as_alias alias_name
%type <astbase_ptr> select_stmt opt_where opt_groupby opt_having opt_orderby 
%type <astbase_ptr> groupby_list orderby_list
%type <astbase_ptr> select_expr select_expr_list 
%type <astbase_ptr> table_factor join_table table_reference table_references 
%type <astbase_ptr> expr
%type <astbase_ptr> opt_join_condition join_condition
%type <astbase_ptr> stmt stmt_list create_table_stmt create_col_list table_subquery create_definition opt_column_list
%type <astbase_ptr> insert_stmt ctext_row ctext_expr_list ctext_expr column_list
%type <astbase_ptr> update_stmt set_clause_list set_clause set_target opt_from_caluse
%type <astbase_ptr> load_stmt opt_column_name_list column_name_list file_path stdin_or_file_path
%type <astbase_ptr> boolean_expression
%{

#include "parser_context.h"
#include "driver.h"
#include "scanner.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex ctx.driver_.lexer->lex

%}

%% /*** Grammar Rules ***/


start:
  stmt SQLEND { ctx.root_ = $1; }
  | stmt_list SQLEND { ctx.root_ = $1; }
;

stmt_list:
  stmt SEMICOLON { $$ = $1; }
  | stmt_list stmt SEMICOLON { $1->RAppend($2); $$ = $1; }
;

boolean_expression:
  expr EQUAL expr {  
    $$ = ctx.NewExpression(kEqual, $1, $3); 
    }
  | boolean_expression ANDOP boolean_expression {
    $$ = ctx.NewExpression(kAnd, $1, $3); 
    }
  | boolean_expression OR boolean_expression {  
    $$ = ctx.NewExpression(kOr, $1, $3); 
    }
  | boolean_expression XOR boolean_expression {  
    $$ = ctx.NewExpression(kXor, $1, $3); 
    }
  | NOT boolean_expression { 
    $$ = ctx.NewExpression(kNot, $2, NULL);
    };
  
expr: expr '+' expr { yyerror("not implement"); YYERROR; }
  | expr '-' expr { yyerror("not implement"); YYERROR; }
  | expr '*' expr { yyerror("not implement"); YYERROR; }
  | expr '/' expr {  }
  | column_name      
  { 
    $$ = ctx.NewColumnReference(NULL, $1);
  }

  | table_name POINT column_name 
  {
    $$ = ctx.NewColumnReference($1, $3);
  }
  
  | USERVAR    
  {
  }

  | STRING    
  { 
    $$ = ctx.NewConstValue($1);
  }
  | INTNUM    { $$ = ctx.NewConstValue($1);}
  | APPROXNUM  {  }
  | BOOL      {   }
;
val_list: expr { }
  | expr COMMA val_list { }
;

opt_val_list: { }
  | val_list
;

expr: expr IN PAREN_LEFT val_list PAREN_RIGHT { }
  | expr NOT IN PAREN_LEFT val_list PAREN_RIGHT { }
  | expr IN PAREN_LEFT select_stmt PAREN_RIGHT  { }
  | expr NOT IN PAREN_LEFT select_stmt PAREN_RIGHT { }
  | EXISTS PAREN_LEFT select_stmt PAREN_RIGHT { }
;

expr: NAME PAREN_LEFT opt_val_list PAREN_RIGHT { }
;

expr: FCOUNT PAREN_LEFT '*' PAREN_RIGHT { }
  | FCOUNT PAREN_LEFT expr PAREN_RIGHT { }
;

expr: FSUBSTRING PAREN_LEFT val_list PAREN_RIGHT  { }
  | FSUBSTRING PAREN_LEFT expr FROM expr PAREN_RIGHT { }
  | FSUBSTRING PAREN_LEFT expr FROM expr FOR expr PAREN_RIGHT {}
;

expr: FTRIM PAREN_LEFT val_list PAREN_RIGHT { }
  | FTRIM PAREN_LEFT trim_ltb expr FROM val_list PAREN_RIGHT { }
;

trim_ltb: LEADING { }
    | TRAILING { }
    | BOTH { }
;

expr: FDATE_ADD PAREN_LEFT expr COMMA interval_exp PAREN_RIGHT { }
  | FDATE_SUB PAREN_LEFT expr COMMA interval_exp PAREN_RIGHT { }
;

interval_exp: INTERVAL expr DAY_HOUR {  }
  | INTERVAL expr DAY_MICROSECOND {  }
  | INTERVAL expr DAY_MINUTE {  }
  | INTERVAL expr DAY_SECOND {  }
  | INTERVAL expr YEAR_MONTH {  }
  | INTERVAL expr YEAR {  }
  | INTERVAL expr HOUR_MICROSECOND {  }
  | INTERVAL expr HOUR_MINUTE {  }
  | INTERVAL expr HOUR_SECOND {  }
;

expr: CASE expr case_list END {  }
  | CASE expr case_list ELSE expr END {  }
  | CASE case_list END {  }
  | CASE case_list ELSE expr END {  }
;

case_list: WHEN expr THEN expr { $$ = 1; }
  | case_list WHEN expr THEN expr { $$ = $1+1; }
;

expr: expr LIKE expr {  }
  | expr NOT LIKE expr {  }
;

expr: expr REGEXP expr {  }
  | expr NOT REGEXP expr {  }
;

expr: CURRENT_TIMESTAMP {  }
  | CURRENT_DATE {  }
  | CURRENT_TIME {  }
;

expr: BINARY expr %prec UMINUS {  }
;

stmt: select_stmt { $$ = $1; }
;

/* simple select with no tables */
select_stmt:
  SELECT select_opts select_expr_list 
  {
     
    $$ = NULL;
  }
  
  | SELECT select_opts select_expr_list FROM table_references 
    opt_where opt_groupby opt_having opt_orderby opt_limit
  { 
    $$ = ctx.NewSelectStmt($3, $5, $6, $7, $8, $9);
  }
;

opt_where:
  { $$ = NULL; }

  | WHERE boolean_expression 
  { $$ = $2; }
;

opt_groupby:
  /* nil */ { $$ = NULL; }

  | GROUP BY groupby_list 
  { $$ = $3; }
;

groupby_list:
  expr 
  {
    $$ = $1; 
  }
  
  | groupby_list COMMA expr 
  { 
    $$ = $1;
    $$->RAppend($3);
  }
  
orderby_list: 
  expr opt_asc_desc 
  {
    $$ = ctx.NewOrderClause($1, (OrderType)$2); 
  }
  
  | orderby_list COMMA expr opt_asc_desc 
  { 
    ASTBase *ptr = ctx.NewOrderClause($3, (OrderType)$4);
    $1->RAppend(ptr);
    $$ = $1;
  }
;

opt_asc_desc: 
  /* nil */ { $$ = kAsc; }
  | ASC { $$ = kAsc; }
;

opt_having: 
  /* nil */ 
  { $$ = NULL; }

  | HAVING expr 
  { $$ = $2; }
;

opt_orderby: 
  /* nil */
  { $$ = NULL; }

  | ORDER BY orderby_list
  { $$ = $3; }
;

opt_limit:
  /* nil */ 
  | LIMIT expr {  }
  | LIMIT expr COMMA expr {  }
;

column_list:
  NAME 
  {
      
     $$ = ctx.NewReferenceName($1);
  }
  
  | column_list COMMA NAME 
  {
    ASTBase *l = $1;
    l->RAppend(ctx.NewReferenceName($3));
    
    $$ = l;
  }
;

select_opts: { $$ = 0; }
| select_opts ALL { if($1 & 01) yyerror("duplicate ALL option"); $$ = $1 | 01; }
| select_opts DISTINCT { if($1 & 02) yyerror("duplicate DISTINCT option"); $$ = $1 | 02; }
| select_opts DISTINCTROW { if($1 & 04) yyerror("duplicate DISTINCTROW option"); $$ = $1 | 04; }
| select_opts HIGH_PRIORITY { if($1 & 010) yyerror("duplicate HIGH_PRIORITY option"); $$ = $1 | 010; }
| select_opts STRAIGHT_JOIN { if($1 & 020) yyerror("duplicate STRAIGHT_JOIN option"); $$ = $1 | 020; }
| select_opts SQL_SMALL_RESULT { if($1 & 040) yyerror("duplicate SQL_SMALL_RESULT option"); $$ = $1 | 040; }
| select_opts SQL_BIG_RESULT { if($1 & 0100) yyerror("duplicate SQL_BIG_RESULT option"); $$ = $1 | 0100; }
| select_opts SQL_CALC_FOUND_ROWS { if($1 & 0200) yyerror("duplicate SQL_CALC_FOUND_ROWS option"); $$ = $1 | 0200; }
;

select_expr_list:
  select_expr 
  {
    $$ = $1;
  }
  
  | select_expr_list COMMA select_expr 
  { 
     
    $1->RAppend($3); $$ = $1; 
  }
  
  | '*' {  $$ = NULL; }
;

select_expr:
  expr opt_as_alias 
  { 
    $$ = ctx.NewSelectTarget($1, $2);
  };


table_references: table_reference 
  { 
    $$  = $1 ;
  }
  
  | table_references COMMA table_reference 
  { 
    ASTBase *l = $1;
    l->RAppend($3);
    $$ = l; 
  }
;

table_reference:
  table_factor { $$ = $1; }
  | join_table { $$ = $1; }
;

table_factor: table_name opt_as_alias index_hint 
  {
    $$ = ctx.NewTableReference($1, $2);
  }

  | table_subquery opt_as alias_name 
  { 
    $$ = ctx.NewSubquery($1, $3);
  }

  | PAREN_LEFT table_references PAREN_RIGHT 
  { 
    $$ = $2;
  }
;

opt_as: AS     
    | /* nil */ 
;

join_table: 
  table_reference opt_join_type JOIN table_factor join_condition
  {
    $$ = ctx.NewJoinClause($1, $4, $5, (JoinType)$2); 
  }

  
  | table_reference NATURAL opt_join_type JOIN table_factor 
  {
    $$ = ctx.NewJoinClause($1, $5, NULL, (JoinType)$3); 
  }

;

opt_outer:
  /* nil */ { }
  | OUTER { $$ = kJoinFullOutter;  }
;

opt_join_type:
  /* nil */ { $$= kJoinInner;  }
  | INNER { $$ = kJoinInner; }
  | CROSS { $$ = kJoinCross; }
  | LEFT opt_outer { $$ = kJoinLeftOutter; }
  | RIGHT opt_outer { $$ = kJoinRightOutter; }
  | OUTER { $$ = kJoinFullOutter;  }
;

opt_join_condition: 
  /* nil */ { $$ = NULL; }
  | join_condition { $$ = $1; };

join_condition: 
  ON boolean_expression { $$ = $2; }
;

index_hint: 
  USE KEY opt_for_join PAREN_LEFT index_list PAREN_RIGHT {  }
  | IGNORE KEY opt_for_join PAREN_LEFT index_list PAREN_RIGHT {  }
  | FORCE KEY opt_for_join PAREN_LEFT index_list PAREN_RIGHT {  }
  | /* nil */
;

opt_for_join: FOR JOIN { $$ = 1; }
    | /* nil */ { $$ = 0; }
;

index_list:
  NAME {  $$ = 1; }
  | index_list COMMA NAME {  $$ = $1 + 1; }
;

table_subquery:
  PAREN_LEFT select_stmt PAREN_RIGHT { $$ = $2; }
;

stmt: delete_stmt {  }
;

delete_stmt:
  DELETE FROM table_references opt_where {  }
;


stmt:
  insert_stmt { $$ = $1; }
;

insert_stmt:
  INSERT INTO table_name opt_column_list VALUES ctext_row
  {
      $$ = ctx.NewInsertStmt($3,$4,$6);
  }
;

ctext_row:
  PAREN_LEFT ctext_expr_list PAREN_RIGHT {  $$ = $2; }
;

ctext_expr_list:
  ctext_expr { $$ = $1; }
  | ctext_expr_list COMMA ctext_expr { $1->RAppend($3); $$ = $1; }
;

ctext_expr: 
  STRING { $$ = ctx.NewConstValue($1); };

set_clause_list:
  set_clause_list COMMA set_clause
  {
    $1->RAppend($3);
    $$ = $1;
  }
;

set_clause:
  set_target EQUAL expr 
  {
    $$ = ctx.NewExpression(kAssign, $1, $3);
  }
;

stmt: update_stmt { $$ = $1; };

update_stmt:
  UPDATE table_name SET set_clause_list opt_from_caluse opt_where
  {
      $$ = ctx.NewUpdateStmt($2, $4, $5, $6); 
  }
;

opt_from_caluse:
  /*EMPTY*/ { $$ = NULL; }
  | FROM table_references { $$ = $2; };
    
set_target:
     NAME
     {
        $$ = ctx.NewReferenceName($1);
     }
     
stmt: create_database_stmt {  };

create_database_stmt: CREATE DATABASE NAME
      {  }
    | CREATE SCHEMA NAME
      { }
;


stmt: create_table_stmt { $$ = $1; }
  ;

create_table_stmt: 
  CREATE TABLE table_name  PAREN_LEFT  create_col_list PAREN_RIGHT
  {
    $$ = ctx.NewCreateStmt($3, $5);
  }
  
  | CREATE TABLE table_name AS table_subquery
  {
    $$ = ctx.NewCreateAsStmt($3, $5);
  }
;

opt_column_list:
  /* nil */ { $$ = NULL; }
  | PAREN_LEFT column_list PAREN_RIGHT 
  { $$ = $2; }


create_col_list:
    create_definition { $$ = $1; }
    | create_col_list COMMA create_definition { $$ = $1; $1->RAppend($3); }
;


create_definition:
  column_name data_type column_atts
  { $$ = ctx.NewColumnDefine($1, $2); }
;

column_atts:
  /* nil */ { $$ = 0; }
  | column_atts NOT NULLX {}
  | column_atts NULLX {}
  | column_atts DEFAULT STRING {}
  | column_atts DEFAULT INTNUM {}
  | column_atts DEFAULT APPROXNUM {}
  | column_atts DEFAULT BOOL {}
  | column_atts AUTO_INCREMENT {}
  | column_atts UNIQUE PAREN_LEFT column_list PAREN_RIGHT {}
  | column_atts UNIQUE KEY {}
  | column_atts PRIMARY KEY {}
  | column_atts KEY {}
  | column_atts COMMENT STRING {}
;


data_type:
  INTEGER { $$ = MAKE_INT32(8, kDTInteger); }
  | FLOAT { $$ = MAKE_INT32(8, kDTFloat); }
  | VARCHAR PAREN_LEFT INTNUM PAREN_RIGHT { $$ = MAKE_INT32($3, kDTVarchar); }
;

table_name:
  NAME      
  { 
    $$ = ctx.NewReferenceName($1);
  }
  
column_name:
  NAME      
  { 
    $$ = ctx.NewReferenceName($1);
  }

alias_name:
  NAME      
  { 
    $$ = ctx.NewReferenceName($1);
  }
  
opt_as_alias: 
  AS alias_name { $$ = $2; }
  | alias_name { $$ = $1; }
  | /* nil */ { $$ = NULL; }
;


stmt: set_stmt {  };

set_stmt: SET set_list ;

set_list: set_expr | set_list COMMA set_expr ;

set_expr: USERVAR COMPARISON expr {
 if ($2 != 4) { }  }
  | USERVAR ASSIGN expr {  }
;

stmt: load_stmt { $$ = $1; };

load_stmt:
    COPY table_name opt_column_name_list from_or_to stdin_or_file_path copy_delimiter opt_copy_options { 
        if ($4 == kFrom)
            $$ = ctx.NewLoadStmt($2, $3, $5); 
        else if ($4 == kTo)
            $$ = ctx.NewExportStmt($2, $3, $5);
    }
    ;
    
from_or_to:
    FROM {
        $$ = kFrom;
    }
    | TO {
        $$ = kTo;
    }
    ;
    
stdin_or_file_path:
    file_path { $$ = $1; }
    | STDIN { $$ = NULL;  }
    | STDOUT { $$ = NULL; }
    ;
    
opt_copy_options:
    opt_csv opt_quote_as
    {}

opt_csv:
    CSV {}
    | /*empty*/ {}

opt_quote_as:
    QUOTE opt_as STRING {}
    | /*empty*/ {}

copy_delimiter:
    opt_using DELIMITER opt_as STRING
                {
                }
    | /*EMPTY*/ { }
    ;

opt_using:
    USING  {}
    | /*EMPTY*/  {}
        ;
        
file_path:
  STRING {
    $$ = ctx.NewConstValue($1);
  };

opt_column_name_list:
  /* nil */ { $$ = NULL; }
  | PAREN_LEFT column_name_list PAREN_RIGHT 
  { $$ = $2; };


column_name_list:
  column_name 
  {   
     $$ = $1;
  }
  
  | column_name_list COMMA column_name 
  {
    $1->RAppend($3);  
    $$ = $1;
  }
;

 /*** END PARSER - Change the parser grammar rules above ***/

%% /*** Additional Code ***/

void parser::Parser::error(const Parser::location_type& l,
        const std::string& m)
{
  ctx.driver_.error(l, m);
}

