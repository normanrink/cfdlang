
  /***************/
  /* DEFINITIONS */
  /***************/

  /* only process single input files: */
%option noyywrap

  /* make 'yytext' a character pointer: */
%option pointer

  /* generate C++ output: */
  /* %option c++ */

%{
  /******************/
  /* C DECLARATION: */
  /******************/

  #include <stdio.h>
  #include <stdlib.h>

  #include "token.h"
  #ifndef _LEXER_STANDALONE_
    #include "ast_c.h"
    #include "lang.tab.h"
  #endif /* _LEXER_STANDALONE_ */
%}

KW_VAR    "var"
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

{KW_VAR}  { return KW_VAR; }
{KW_TYPE} { return KW_TYPE; }
{COLON}   { return COLON; }
{LPAREN}  { return LPAREN; }
{RPAREN}  { return RPAREN; }
{LBRACK}  { return LBRACK; }
{RBRACK}  { return RBRACK; }
{STAR}    { return STAR; }
{DOT}     { return DOT; }
{EQUAL}   { return EQUAL; }
{INT}     { 
            #ifndef _LEXER_STANDALONE_
            yylval.integer = atoi(yytext);
            #endif /* _LEXER_STANDALONE_ */
            return INT;
          }
{ID}      {
            #ifndef _LEXER_STANDALONE_
            yylval.string = yytext;
            #endif /* _LEXER_STANDALONE_ */
            return ID;
          }
<<EOF>>   { return end_of_file; }
[ \t\n]+  /* ignore whitespace */ 

%%

  /**************/
  /* USER CODE: */
  /**************/

#ifdef _LEXER_STANDALONE_

int main(int argc, char* argv[]) {
  FILE *in;
  int tok;

  if (argc != 2) {
    return 1;
  } else {
    in = fopen(argv[1], "r");
    if (!in)
      return 2;
  }

  yyin = in;
  while(1) {
    tok = yylex();
    if (tok == end_of_file)
      break;

    printf("%s ", token_string[tok]);
  }
  printf("\n");

  fclose(in);

  return 0;
}

#endif /* _LEXER_STANDALONE_ */

