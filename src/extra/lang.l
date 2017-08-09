
  /***************/
  /* DEFINITIONS */
  /***************/

  /* only process single input files: */
%option noyywrap

  /* make 'yytext' a character pointer: */
%option pointer

  /* generate a re-entrant scanner: */
%option reentrant bison-bridge

%{
  /******************/
  /* C DECLARATION: */
  /******************/

  //#include "AST/AST.h"
  #include "lang.tab.hh"
%}

KW_VAR    "var"
KW_INPUT  "input"
KW_OUTPUT "output"
KW_TYPE   "type"
KW_ELEM   "elem"
KW_FIRST  "first"
KW_LAST   "last"
COLON     ":"
LPAREN    "("
RPAREN    ")"
LBRACK    "["
RBRACK    "]"
DOT       "."
PLUS      "+"
DASH      "-"
STAR      "*"
SLASH     "/"
HASH      "#"
CARET     "\^"
EQUAL     "="
INT       [0-9]+
ID        [_a-zA-Z][_a-zA-Z0-9]*

%%

  /**********/
  /* RULES: */
  /**********/

{KW_VAR}    { return KW_VAR; }
{KW_INPUT}  { return KW_INPUT; }
{KW_OUTPUT} { return KW_OUTPUT; }
{KW_TYPE}   { return KW_TYPE; }
{KW_ELEM}   { return KW_ELEM; }
{KW_FIRST}  { return KW_FIRST; }
{KW_LAST}   { return KW_LAST; }
{COLON}     { return COLON; }
{LPAREN}    { return LPAREN; }
{RPAREN}    { return RPAREN; }
{LBRACK}    { return LBRACK; }
{RBRACK}    { return RBRACK; }
{DOT}       { return DOT; }
{PLUS}      { return PLUS; }
{DASH}      { return DASH; }
{STAR}      { return STAR; }
{SLASH}     { return SLASH; }
{HASH}      { return HASH; }
{CARET}     { return CARET; }
{EQUAL}     { return EQUAL; }
{INT}       { 
              yylval->integer_literal = atoi(yytext);
              return INT;
            }
{ID}        {
              yylval->string_literal = yytext;
              return ID;
            }
<<EOF>>     { return EOF; }
[ \t\n]+    /* ignore whitespace */ 

%%

  /**************/
  /* USER CODE: */
  /**************/

