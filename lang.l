
  /***************/
  /* DEFINITIONS */
  /***************/

  /* only process single input files: */
%option noyywrap

  /* make 'yytext' a character pointer: */
%option pointer

  /* generate C++ output: */
%option c++

%option yyclass="myFlexLexer"

%{
  /******************/
  /* C DECLARATION: */
  /******************/

  #include <fstream>
  #include <iostream>
  #include <map>
  #include <string>

  #include "ast.h"

  #include "lang.tab.hh"

  #define TOKEN(t) (yy::parser::token::t)

  std::map<const int,  const std::string> token_string = {
    { TOKEN(KW_VAR), "KW_VAR" },
    { TOKEN(KW_TYPE), "KW_TYPE" },
    { TOKEN(COLON), "COLON" },
    { TOKEN(LPAREN), "LPAPREN" },
    { TOKEN(RPAREN), "RPAREN" },
    { TOKEN(LBRACK), "LBRACK" },
    { TOKEN(RBRACK), "RBRACK" },
    { TOKEN(STAR), "STAR" },
    { TOKEN(DOT), "DOT" },
    { TOKEN(EQUAL), "EQUAL" },
    { TOKEN(INT), "INT" },
    { TOKEN(ID), "ID" }
  };
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

{KW_VAR}  { return TOKEN(KW_VAR); }
{KW_TYPE} { return TOKEN(KW_TYPE); }
{COLON}   { return TOKEN(COLON); }
{LPAREN}  { return TOKEN(LPAREN); }
{RPAREN}  { return TOKEN(RPAREN); }
{LBRACK}  { return TOKEN(LBRACK); }
{RBRACK}  { return TOKEN(RBRACK); }
{STAR}    { return TOKEN(STAR); }
{DOT}     { return TOKEN(DOT); }
{EQUAL}   { return TOKEN(EQUAL); }
{INT}     { return TOKEN(INT); }
{ID}      { return TOKEN(ID); }
[ \t\n]+  /* ignore whitespace */ 
<<EOF>>   { return EOF; }

%%

  /**************/
  /* USER CODE: */
  /**************/

#ifdef _LEXER_STANDALONE_

int main(int argc, char* argv[]) {
  myFlexLexer lexer;
  int tok;

  std::ifstream in;

  if (argc != 2) {
    return 1;
  } else {
    in.open(argv[1]);
    if (!in.is_open())
      return 2;
  }

  lexer.switch_streams(&in);
  while((tok = lexer.yylex()) != EOF)
    std::cout << token_string[tok] << " ";
  std::cout << "\n";

  in.close();

  return 0;
}

#endif /* _LEXER_STANDALONE_ */

