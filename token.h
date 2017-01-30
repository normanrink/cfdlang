
#ifndef __TOKEN_H__
#define __TOKEN_H__

#ifdef _LEXER_STANDALONE_

#include <string>

enum Token {
  KW_VAR,
  KW_TYPE,
  COLON,
  LPAREN,
  RPAREN,
  LBRACK,
  RBRACK,
  STAR,
  DOT,
  EQUAL,
  INT,
  ID,
  TOKEN_COUNT
};

const std::string token_string[TOKEN_COUNT] = {
  "KW_VAR",
  "KW_TYPE",
  "COLON",
  "LPAPREN",
  "RPAREN",
  "LBRACK",
  "RBRACK",
  "STAR",
  "DOT",
  "EQUAL",
  "INT",
  "ID"
};
#endif /* _LEXER_STANDALONE_ */

#endif /* __TOKEN_H__ */

