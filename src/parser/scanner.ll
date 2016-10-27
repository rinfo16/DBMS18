/* $Id$ -*- mode: c++ -*- */
/** \file scanner.ll Define the parser Flex lexical scanner */

%{ /*** C/C++ Declarations ***/

#include <string>

#include "scanner.h"
#include <stdarg.h>
#include <string.h>

void yyerror(const char *s, ...);
int oldstate;

/* import the parser's token type into a local typedef */
typedef parser::Parser::token token;
typedef parser::Parser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::SQLEND

/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

%}

/*** Flex Declarations and Options ***/

/* enable c++ scanner class generation */
%option c++

/* change the name of the scanner class. results in "SQLFlexLexer" */
%option prefix="SQL"

/* the manual says "somewhat more optimized" */
%option batch

/* enable scanner to generate debug output. disable this for release
 * versions. */
%option debug

/* no support for include files is planned */
%option yywrap nounput 

/* enables the use of start condition stacks */
%option stack

%option nodefault yylineno case-insensitive

/* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
%}

%x CCOMMENT
%s BTWMODE

%% /*** Regular Expressions Part ***/

 /* code to place at the beginning of yylex() */
%{
    // reset location
    yylloc->step();
%}

 /*** BEGIN PARSER - Change the parser lexer rules below ***/



AND { return token::ANDOP; }
ANY { return token::ANY; }
AS { return token::AS; }
ASC { return token::ASC; }
BETWEEN { BEGIN BTWMODE; return token::BETWEEN; }
INT8|BIGINT { return token::BIGINT; }
BOTH { return token::BOTH; }
COLUMN { return token::COLUMN; }
CREATE { return token::CREATE; }
CROSS { return token::CROSS; }
DATABASE { return token::DATABASE; }
DATABASES { return token::DATABASES; }
DATE { return token::DATE; }
DATETIME { return token::DATETIME; }
DAY_HOUR { return token::DAY_HOUR; }
DAY_MICROSECOND { return token::DAY_MICROSECOND; }
DAY_MINUTE { return token::DAY_MINUTE; }
DAY_SECOND { return token::DAY_SECOND; }
NUMERIC|DEC|DECIMAL { return token::DECIMAL; }
DECLARE { return token::DECLARE; }
DEFAULT { return token::DEFAULT; }
DELAYED { return token::DELAYED; }
DELETE { return token::DELETE; }
DESC { return token::DESC; }
DISTINCT { return token::DISTINCT; }
FLOAT8|DOUBLE { return token::DOUBLE; }
DROP { return token::DROP; }
DUAL { return token::DUAL; }
EACH { return token::EACH; }
ELSE { return token::ELSE; }
ELSEIF { return token::ELSEIF; }
END { return token::END; }
EXISTS { yylval->subtok = 0; return token::EXISTS; }
NOT[ \t\n]+EXISTS { yylval->subtok = 1; return token::EXISTS; }
EXIT { return token::EXIT; }
EXPLAIN { return token::EXPLAIN; }
FETCH { return token::FETCH; }
FLOAT4? { return token::FLOAT; }
FOR { return token::FOR; }
FORCE { return token::FORCE; }
FOREIGN { return token::FOREIGN; }
FROM { return token::FROM; }
GRANT { return token::GRANT; }
GROUP { return token::GROUP; }
HAVING { return token::HAVING; }
IN { return token::IN; }
INSERT { return token::INSERT; }
INT4?|INTEGER { return token::INTEGER; }
INTO { return token::INTO; }
IS { return token::IS; }
JOIN { return token::JOIN; }
INDEX|KEY { return token::KEY; }
KEYS { return token::KEYS; }
LEFT { return token::LEFT; }
LIKE { return token::LIKE; }
LIMIT { return token::LIMIT; }
NATURAL { return token::NATURAL; }
NOT { return token::NOT; }
NO_WRITE_TO_BINLOG { return token::NO_WRITE_TO_BINLOG; }
NULL { return token::NULLX; }
NUMBER { return token::NUMBER; }
ON { return token::ON; }
ON[ \t\n]+DUPLICATE { return token::ONDUPLICATE; }
OR { return token::OR; }
OUT { return token::OUT; }
OUTER { return token::OUTER; }
READS { return token::READS; }
REAL { return token::REAL; }
SCHEMA { return token::SCHEMA; }
SET { return token::SET; }
SHOW { return token::SHOW; }
INT2|SMALLINT { return token::SMALLINT; }
SOME { return token::SOME; }
SQL { return token::SQL; }
TABLE { return token::TABLE; }
TIMESTAMP { return token::TIMESTAMP; }
INT1|TINYINT { return token::TINYINT; }
TINYTEXT { return token::TINYTEXT; }
USING { return token::USING; }
VALUES? { return token::VALUES; }
VARBINARY { return token::VARBINARY; }
VARCHAR(ACTER)? { return token::VARCHAR; }
VARYING { return token::VARYING; }
XOR { return token::XOR; }

-?[0-9]+ { yylval->intval = atoi(yytext); return token::INTNUM; }
-?[0-9]+"."[0-9]* |
-?"."[0-9]+ |
-?[0-9]+E[-+]?[0-9]+ |
-?[0-9]+"."[0-9]*E[-+]?[0-9]+ |
-?"."[0-9]+E[-+]?[0-9]+ { yylval->floatval = atof(yytext) ;
                          return token::APPROXNUM; }

TRUE { yylval->intval = 1; return token::BOOL; }
UNKNOWN { yylval->intval = -1; return token::BOOL; }
FALSE { yylval->intval = 0; return token::BOOL; }

'(\\.|''|[^'\n])*' |
\"(\\.|\"\"|[^"\n])*\" { yylval->strval = strdup(yytext); return token::STRING; }

'(\\.|[^'\n])*$ { yyerror("Unterminated string %s", yytext); }

\"(\\.|[^"\n])*$ { yyerror("Unterminated string %s", yytext); }

X'[0-9A-F]+' |
0X[0-9A-F]+ { yylval->strval = strdup(yytext); return token::STRING; }
0B[01]+ |
B'[01]+' { yylval->strval = strdup(yytext); return token::STRING; }


"&&" { return token::ANDOP; }
"||" { return token::OR; }

"=" { yylval->subtok = 4; return token::COMPARISON; }
"<=>" { yylval->subtok = 12; return token::COMPARISON; }
">=" { yylval->subtok = 6; return token::COMPARISON; }
">" { yylval->subtok = 2; return token::COMPARISON; }
"<=" { yylval->subtok = 5; return token::COMPARISON; }
"<" { yylval->subtok = 1; return token::COMPARISON; }

"!="|"<>" { yylval->subtok = 3; return token::COMPARISON; }
"<<" { yylval->subtok = 1; return token::SHIFT; }
">>" { yylval->subtok = 2; return token::SHIFT; }
":=" { return token::ASSIGN; }
"," { return token::COMMA; }
";" { return token::SEMICOLON; }
"(" { return token::PAREN_LEFT; }
")" { return token::PAREN_RIGHT; }

SUBSTR(ING)?/"(" { return token::FSUBSTRING; }
TRIM/"(" { return token::FTRIM; }
DATE_ADD/"(" { return token::FDATE_ADD; }
DATE_SUB/"(" { return token::FDATE_SUB; }

COUNT { int c; // = input(); unput(c);
        if(c == '(') return token::FCOUNT;
        yylval->strval = strdup(yytext);
        return token::NAME; }

[A-Za-z][A-Za-z0-9_]* { yylval->strval = strdup(yytext); return token::NAME; }

`[^`/\\.\n]+` { yylval->strval = strdup(yytext+1); yylval->strval[yyleng-2] = 0; return token::NAME; }

`[^`\n]*$ { yyerror("unterminated quoted name %s", yytext); }

@[0-9a-z_.$]+ |
@\"[^"\n]+\" |
@`[^`\n]+` |
@'[^'\n]+' { yylval->strval = strdup(yytext+1); return token::USERVAR; }

@\"[^"\n]*\n |
@`[^`\n]*\n |
@'[^'\n]*\n { yyerror("unterminated quoted user variable %s", yytext); }

#.* ;
"--"[ \t].* ;

"/*" { oldstate = YY_START; BEGIN CCOMMENT; }
<CCOMMENT>"*/" { BEGIN oldstate; }
<CCOMMENT>.|\n ;

<CCOMMENT><<EOF>> { yyerror("unclosed comment"); }

[ \t\n] /* whitespace */
. { /*yyerror("mystery character '%c'", *yytext);*/ }


 /*** END PARSER - Change the parser lexer rules above ***/

%% /*** Additional Code ***/

namespace parser {

Scanner::Scanner(std::istream* in,
     std::ostream* out)
    : SQLFlexLexer(in, out)
{
}

Scanner::~Scanner()
{
}

void Scanner::set_debug(bool b)
{
    yy_flex_debug = b;
}

}

/* This implementation of SQLFlexLexer::yylex() is required to fill the
 * vtable of the class SQLFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the Scanner class instead. */

#ifdef yylex
#undef yylex
#endif

int SQLFlexLexer::yylex()
{
    std::cerr << "in SQLFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int SQLFlexLexer::yywrap()
{
    return 1;
}
