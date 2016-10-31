/* $Id$ -*- mode: c++ -*- */
/** \file parser.yy Contains the parser Bison parser source */

%{ /*** C/C++ Declarations ***/

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include "parser/ast_base.h"
#include "select_stmt.h"
#include "name.h"
#include "parser_context.h"

#define yyerror(s) (ctx.driver_.error(s))
#define YYDEBUG 1


%}

/*** yacc/bison Declarations ***/

/* Require bison 2.1 or later */
%require "2.1"

/* add debug output code to generated parser. disable this for release
 * versions. */
%debug


/* write out a header file containing the token defines */
%defines

/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
%define api.prefix {parser}


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
  int intval;
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
%token <strval> INTNUM
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
%token CREATE
%token CROSS
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

%type <strval> opt_as_alias
%type <intval> select_opts
%type <intval> val_list opt_val_list case_list
%type <intval> opt_asc_desc opt_table_scope
%type <intval> opt_inner_cross opt_outer
%type <intval> left_or_right opt_left_or_right_outer 
%type <intval> index_list opt_for_join
%type <intval> column_atts data_type 

%type <astbase_ptr> talbe_name column_name
%type <astbase_ptr> select_stmt opt_where opt_groupby opt_having opt_orderby groupby_list 
%type <astbase_ptr>  select_expr select_expr_list 
%type <astbase_ptr> table_factor join_table table_reference table_references expr
%type <astbase_ptr> opt_join_condition join_condition opt_with_rollup
%type <astbase_ptr> stmt stmt_list create_table_stmt create_col_list table_subquery create_definition opt_column_list
%type <astbase_ptr> insert_stmt ctext_row ctext_expr_list ctext_expr column_list
%type <astbase_ptr> update_stmt set_clause_list set_clause set_target opt_from_caluse
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
  stmt_list SQLEND { ctx.root_ = $1; }
;

stmt_list:
  stmt SEMICOLON { $$ = $1; }
  | stmt_list stmt SEMICOLON {}
;

expr: expr '+' expr { yyerror("not implement"); YYERROR; }
  | expr '-' expr { yyerror("not implement"); YYERROR; }
  | expr '*' expr { yyerror("not implement"); YYERROR; }
  | expr '/' expr {  }
  | expr '%' expr { }
  | expr MOD expr { }
  | '-' expr %prec UMINUS { }
  | expr ANDOP expr { }
  | expr OR expr { }
  | expr XOR expr { }
  | expr '|' expr { }
  | expr '&' expr { }
  | expr '^' expr { }
  | expr SHIFT expr { }
  | NOT expr { }
  | '!' expr { }
  | expr COMPARISON expr { }
  /* recursive selects and comparisons thereto */
  | expr COMPARISON PAREN_LEFT select_stmt PAREN_RIGHT { }
  | expr COMPARISON ANY PAREN_LEFT select_stmt PAREN_RIGHT { }
  | expr COMPARISON SOME PAREN_LEFT select_stmt PAREN_RIGHT { }
  | expr COMPARISON ALL PAREN_LEFT select_stmt PAREN_RIGHT { }
  | expr IS NULLX { }
  | expr IS NOT NULLX { }
  | expr IS BOOL { }
  | expr IS NOT BOOL { }
  | USERVAR ASSIGN expr { }
  |expr BETWEEN expr AND expr %prec BETWEEN 
  {
  }
  
  | NAME      
  { 
    $$ = ctx.new_name($1);
    free($1); 
  }

  | NAME '.' NAME 
  {
    
    free($1);
    free($3); 
  }
  
  | USERVAR    
  {
     
    free($1); 
  }

  | STRING    { $$ = ctx.new_value($1);free($1); }
  | INTNUM    { $$ = ctx.new_value($1);free($1); }
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

expr: NAME PAREN_LEFT opt_val_list PAREN_RIGHT { free($1); }
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
    $$ = ctx.new_select_stmt($3, $5, $6, $7, $8, $9);
  }
;

opt_where:
  { $$ = NULL; }

  | WHERE expr 
  { $$ = $2; }
;

opt_groupby:
  /* nil */ { $$ = NULL; }

  | GROUP BY groupby_list opt_with_rollup 
  { $$ = $3; }
;

groupby_list: 
  expr opt_asc_desc 
  {
    $$ = ctx.new_orderby($1, $2); 
  }
  
  | groupby_list COMMA expr opt_asc_desc 
  { 
    ASTBase *ptr = ctx.new_orderby($3, $4);
    $1->rappend(ptr);
    $$ = $1;
  }
;

opt_asc_desc: 
  /* nil */ { $$ = ORD_ASC; }
  | ASC { $$ = ORD_ASC; }
  | DESC { $$ = ORD_DESC; }
;

opt_with_rollup:
  /* nil */ { $$ = NULL; }
  | WITH ROLLUP { $$ = NULL; }
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

  | ORDER BY groupby_list
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
      
     $$ = ctx.new_name($1);
     free($1); 
  }
  
  | column_list COMMA NAME 
  {
    ASTBase *l = $1;
    l->rappend(ctx.new_name($3));
    
    free($3);
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
     
    $1->rappend($3); $$ = $1; 
  }
  
  | '*' {  $$ = NULL; }
;

select_expr:
  expr opt_as_alias 
  { 
    $$ = $1;
  };

opt_as_alias: 
  AS NAME { $$ = $2; }
  | NAME { $$ = $1; }
  | /* nil */ { $$ = NULL; }
;

table_references: table_reference 
  { 
    $$  = $1 ;
  }
  
  | table_references COMMA table_reference 
  { 
    ASTBase *l = $1;
    l->rappend($3);
    $$ = l; 
  }
;

table_reference:
  table_factor { $$ = $1; }
  | join_table { $$ = $1; }
;

table_factor: NAME opt_as_alias index_hint 
  {
    $$ = ctx.new_table_factor($1, $2, NULL);
      free($1); free($2); 
  }

  | NAME '.' NAME opt_as_alias index_hint 
  { 
    /* TODO DB name ...*/
    $$ = ctx.new_table_factor($3, $4, NULL); 
    free($3); free($4); 
  }

  | table_subquery opt_as NAME 
  { 
    $$ = ctx.new_table_factor(NULL, $3, $1);
    free($3); 
  }

  | PAREN_LEFT table_references PAREN_RIGHT 
  { 
    $$ = ctx.new_table_factor(NULL, NULL, $2);
  }
;

opt_as: AS
    | /* nil */
;

join_table: 
  table_reference opt_inner_cross JOIN table_factor opt_join_condition 
  {
    $$ = ctx.new_join_table($1, $4, $5, $2); 
  }

  | table_reference STRAIGHT_JOIN table_factor 
  { 
    $$ = ctx.new_join_table($1, $3, NULL, JOIN_STRAIGHT);
  }

  | table_reference STRAIGHT_JOIN table_factor ON expr 
  { 
    $$ = ctx.new_join_table($1, $3, $5, JOIN_STRAIGHT); 
  }
  
  | table_reference left_or_right opt_outer JOIN table_factor join_condition
  {
    $$ = ctx.new_join_table($1, $5, $6, $2); 
  }

  
  | table_reference NATURAL opt_left_or_right_outer JOIN table_factor 
  {
    $$ = ctx.new_join_table($1, $5, NULL, $3); 
  }

;

opt_inner_cross: /* nil */ { $$ = JOIN_NONE; }
  | INNER { $$ = JOIN_INNER; }
  | CROSS { $$ = JOIN_CROSS; }
;

opt_outer:
  /* nil */ { }
  | OUTER { }
;

left_or_right: LEFT { $$ = JOIN_LEFT; }
  | RIGHT { $$ = JOIN_RIGHT; }
;

opt_left_or_right_outer: LEFT opt_outer { $$ = JOIN_NATURAL_LEFT; }
  | RIGHT opt_outer { $$ = JOIN_NATURAL_RIGHT; }
  | /* nil */ { $$ = JOIN_NATURAL_INNER; }
;

opt_join_condition: 
  /* nil */ { $$ = NULL; }
  | join_condition { $$ = $1; };

join_condition: 
  ON expr { $$ = $2; }
  | USING PAREN_LEFT column_list PAREN_RIGHT { $$ = $3; }
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

stmt:
  delete_stmt {  }
;

delete_stmt:
  DELETE FROM table_references opt_where {  }
;


stmt:
  insert_stmt { $$ = $1; }
;

insert_stmt:
  INSERT INTO NAME opt_column_list VALUES ctext_row
  {
      $$ = ctx.new_insert_stmt($3,$4,$6);
  }
;

ctext_row:
  PAREN_LEFT ctext_expr_list PAREN_RIGHT {  $$ = $2; }
;

ctext_expr_list:
  ctext_expr { $$ = $1; }
  | ctext_expr_list COMMA ctext_expr { $1->rappend($3); $$ = $1; }
;

ctext_expr: 
  STRING { $$ = ctx.new_value($1); };

set_clause_list:
  set_clause_list COMMA set_clause
  {
    $1->rappend($3);
    $$ = $1;
  }
;

set_clause:
  set_target COMPARISON expr 
  {
    if ($2 != kAssign)
    { 
      
    }
    $$ = ctx.new_expression(kAssign, $1, $3);
  }
;

/** update **/
stmt:
  update_stmt { $$ = $1; };

update_stmt:
  UPDATE talbe_name SET set_clause_list opt_from_caluse opt_where
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
        $$ = ctx.new_name($1);
        free($1); 
     }
     
stmt: create_database_stmt {  };

create_database_stmt: CREATE DATABASE NAME
      { free($3); }
    | CREATE SCHEMA NAME
      { free($3); }
;


stmt: create_table_stmt { }
  ;

create_table_stmt: 
  CREATE opt_table_scope TABLE NAME  PAREN_LEFT  create_col_list PAREN_RIGHT
  {
    $$ = ctx.new_create_table_stmt($4, $2, $6);
    free($4);
  }
  
  | CREATE opt_table_scope TABLE NAME opt_column_list AS table_subquery
  {
    $$ = ctx.new_create_table_as_stmt($4, $2, $5, $7);
    free($4);
  }
;

opt_table_scope: 
  /* nil */ { $$ = 0; }
  | TEMPORARY { $$ = TABLE_SCOPE_TMP; }
;

opt_column_list:
  /* nil */ { $$ = NULL; }
  | PAREN_LEFT column_list PAREN_RIGHT 
  { $$ = $2; }


create_col_list:
    create_definition { $$ = $1; }
    | create_col_list COMMA create_definition { $$ = $1; $1->rappend($3); }
;

create_definition: /* not support */
  PRIMARY KEY PAREN_LEFT column_list PAREN_RIGHT { $$ = NULL; }
  | KEY PAREN_LEFT column_list PAREN_RIGHT { $$ = NULL; }
  | INDEX PAREN_LEFT column_list PAREN_RIGHT { $$ = NULL; }
  | FULLTEXT INDEX PAREN_LEFT column_list PAREN_RIGHT { $$ = NULL; }
  | FULLTEXT KEY PAREN_LEFT column_list PAREN_RIGHT { $$ = NULL; }
;

create_definition:
  NAME data_type column_atts
  { $$ = ctx.new_column_def($1, $2); }
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
  INTEGER { $$ = DATA_TYPE_INTEGER; }
  | FLOAT { $$ = DATA_TYPE_FLOAT; }
;

talbe_name:
  NAME      
  { 
    $$ = ctx.new_name($1);
    free($1); 
  }
  
column_name:
  NAME      
  { 
    $$ = ctx.new_name($1);
    free($1); 
  }
  



stmt: set_stmt {  };

set_stmt: SET set_list ;

set_list: set_expr | set_list COMMA set_expr ;

set_expr: USERVAR COMPARISON expr {
 if ($2 != 4) { }  }
  | USERVAR ASSIGN expr {  }
;

 /*** END PARSER - Change the parser grammar rules above ***/

%% /*** Additional Code ***/

void parser::Parser::error(const Parser::location_type& l,
        const std::string& m)
{
  ctx.driver_.error(l, m);
}

