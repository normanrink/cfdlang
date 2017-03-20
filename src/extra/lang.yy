
/* generate C++ output: */ 
/* %skeleton "lalr1.cc" */

/* generate a pure (i.e. re-entrant) parser: */
%define api.pure full

%param { yyscan_t yyscanner }
%parse-param { const Program *&root }

%code requires {
  #include "AST/AST.h"

  typedef void* yyscan_t;
}

%{
  /******************/
  /* C DECLARATION: */
  /******************/

  #include <stdio.h>


  #include "lang.tab.hh"
  #include "lex.yy.h"


  void yyerror(yyscan_t scanner, const Program *root, const char *msg) {
    fprintf(stderr, "%s\n", msg);
  }
%}

  /***********************/
  /* BISON DECLARATIONS: */
  /***********************/

%union {
  const Program *program;
  
  DeclList *decls;
  StmtList *stmts;
  ExprList *exprs;
 
  const Decl *decl;

  const Stmt *stmt;

  const Expr *expr;

  const Identifier *identifier;
  const Integer *integer;
  const BrackExpr *brack;
  const ParenExpr *paren;

  const char *string_literal;
  int integer_literal;

  int iospec;
}

%token KW_VAR
%token KW_INPUT
%token KW_OUTPUT
%token KW_TYPE
%token COLON
%token LPAREN
%token RPAREN
%token LBRACK
%token RBRACK
%token DOT
%token PLUS
%token DASH
%token STAR
%token SLASH
%token HASH
%token EQUAL
%token <integer_literal> INT
%token <string_literal> ID

%start program

%type <program> program
%type <decls> decl_list
%type <decl> decl var_decl type_decl
%type <stmts> stmt_list
%type <stmt> stmt
%type <expr> type_expr contract_expr expr term factor atom
%type <identifier> identifier
%type <integer> integer
%type <brack> brack_expr
%type <paren> paren_expr
%type <exprs> contract_expr_list
%type <iospec> iospec

%%

  /******************/
  /* GRAMMAR RULES: */
  /******************/

program : decl_list stmt_list { root = Program::create($1, $2); }

decl_list : decl_list decl { $$ = DeclList::append($1, $2); }
          | decl  { $$ = DeclList::create($1); }

decl : var_decl
     | type_decl

var_decl : KW_VAR iospec identifier COLON type_expr {
             $$ = Decl::create(ASTNode::NT_VarDecl,
                               $3, $5,
                               (Decl::IOSpecifier)$2);
           }

iospec : /* empty */ { $$ = Decl::IO_Empty; }
       | KW_INPUT iospec { $$ = Decl::IO_Input | $2; }
       | KW_OUTPUT iospec { $$ = Decl::IO_Output | $2; }

type_decl : KW_TYPE identifier COLON type_expr {
              $$ = Decl::create(ASTNode::NT_TypeDecl, $2, $4);
            }

stmt_list : stmt_list stmt { $$ = StmtList::append($1, $2); }
          | stmt { $$ = StmtList::create($1); }

stmt : identifier EQUAL contract_expr { $$ = Stmt::create($1, $3); }

type_expr : identifier { $$ = (const Expr *)$1; }
          | brack_expr { $$ = (const Expr *)$1; }

contract_expr : expr
              | expr DOT contract_expr {
                  $$ = BinaryExpr::create(ASTNode::NT_ContractionExpr, $1, $3);
                }

expr : term
     | term PLUS expr { $$ = BinaryExpr::create(ASTNode::NT_AddExpr, $1, $3); }
     | term DASH expr { $$ = BinaryExpr::create(ASTNode::NT_SubExpr, $1, $3); }

term : factor
     | factor STAR term {
         $$ = BinaryExpr::create(ASTNode::NT_MulExpr, $1, $3);
       }
     | factor SLASH term {
         $$ = BinaryExpr::create(ASTNode::NT_DivExpr, $1, $3);
       }

factor : atom
       | atom HASH factor {
           $$ = BinaryExpr::create(ASTNode::NT_ProductExpr, $1, $3);
         }

atom : identifier { $$ = (const Expr *)$1; }
     | integer { $$ = (const Expr *)$1; }
     | brack_expr { $$ = (const Expr *)$1; }
     | paren_expr { $$ = (const Expr *)$1; }

identifier : ID { $$ = Identifier::create($1); }

integer : INT { $$ = Integer::create($1); }
     
brack_expr : LBRACK contract_expr_list RBRACK { $$ = BrackExpr::create($2); }
       
paren_expr : LPAREN contract_expr RPAREN { $$ = ParenExpr::create($2); }

contract_expr_list : /* empty */ { $$ = ExprList::create(); }
                   | contract_expr_list contract_expr {
                       $$ = ExprList::append($1, $2);
                     }

%%

  /**************/
  /* USER CODE: */
  /**************/

