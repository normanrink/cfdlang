
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

  const ElemDirect *elem;

  const Stmt *stmt;

  const Expr *expr;

  const Identifier *identifier;
  const Integer *integer;
  const BrackExpr *brack;
  const ParenExpr *paren;

  const char *string_literal;
  int integer_literal;

  int iospec;
  int posspec;
}

%token KW_VAR
%token KW_INPUT
%token KW_OUTPUT
%token KW_TYPE
%token KW_ELEM
%token KW_FIRST
%token KW_LAST
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
%token CARET
%token EQUAL
%token <integer_literal> INT
%token <string_literal> ID

%start program

%type <program> program
%type <decls> decl_list
%type <decl> decl var_decl type_decl
%type <elem> elem_direct
%type <stmts> stmt_list
%type <stmt> stmt
%type <expr> type_expr expr term factor atom
%type <identifier> identifier
%type <integer> integer
%type <brack> brack_expr
%type <paren> paren_expr
%type <exprs> expr_list
%type <iospec> iospec
%type <posspec> posspec

%%

  /******************/
  /* GRAMMAR RULES: */
  /******************/

program : decl_list elem_direct stmt_list {
            root = Program::create($1, $3, $2);
          }

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

stmt : identifier EQUAL expr { $$ = Stmt::create($1, $3); }

type_expr : identifier { $$ = (const Expr *)$1; }
          | brack_expr { $$ = (const Expr *)$1; }

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
     | factor DOT term {
         $$ = BinaryExpr::create(ASTNode::NT_ContractionExpr, $1, $3);
       }

factor : atom
       | atom HASH factor {
           $$ = BinaryExpr::create(ASTNode::NT_ProductExpr, $1, $3);
         }
       | atom CARET factor {
           $$ = BinaryExpr::create(ASTNode::NT_TranspositionExpr, $1, $3);
         }

atom : identifier { $$ = (const Expr *)$1; }
     | integer { $$ = (const Expr *)$1; }
     | brack_expr { $$ = (const Expr *)$1; }
     | paren_expr { $$ = (const Expr *)$1; }

identifier : ID { $$ = Identifier::create($1); }

integer : INT { $$ = Integer::create($1); }
     
brack_expr : LBRACK expr_list RBRACK { $$ = BrackExpr::create($2); }
       
paren_expr : LPAREN expr RPAREN { $$ = ParenExpr::create($2); }

expr_list : /* empty */ { $$ = ExprList::create(); }
          | expr_list expr {
              $$ = ExprList::append($1, $2);
            }

posspec : KW_FIRST { $$ = ElemDirect::POS_First; }
        | KW_LAST { $$ = ElemDirect::POS_Last; }

elem_direct : /* empty */ { $$ = nullptr; }
            | KW_ELEM brack_expr integer posspec {
                $$ = ElemDirect::create(ASTNode::NT_ElemDirect,
                                        (ElemDirect::POSSpecifier)$4, $3, $2);
              }

%%

  /**************/
  /* USER CODE: */
  /**************/

