
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
COLON     ":"
LPAREN    "("
RPAREN    ")"
LBRACK    "["
RBRACK    "]"
STAR      "*"
EQUAL     "="
DOT       "."
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
{COLON}     { return COLON; }
{LPAREN}    { return LPAREN; }
{RPAREN}    { return RPAREN; }
{LBRACK}    { return LBRACK; }
{RBRACK}    { return RBRACK; }
{STAR}      { return STAR; }
{DOT}       { return DOT; }
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

