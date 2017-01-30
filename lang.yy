
/* generate C++ output: */ 
%skeleton "lalr1.cc"

/* generate header file: */
%defines

/* use objects to pass semantic values */
%define api.value.type variant

%code requires {
  class myFlexLexer;
}

%code provides {
  #include "ast.h"

  class myFlexLexer : public yyFlexLexer {
  public:
  #ifndef _LEXER_STANDALONE_
    yy::parser::symbol_type symbol;
  #endif /* !_LEXER_STANDALONE_ */
    virtual int yylex() override;
  };
  
}

%lex-param { lexer }

%parse-param { myFlexLexer &lexer }
%parse-param { const Program *&root }

%{
  /******************/
  /* C DECLARATION: */
  /******************/

  #include <string>
  #include <fstream>
  #include <FlexLexer.h>

  #include "ast.h"

  #include "lang.tab.hh"

  using namespace yy;

  int yylex (parser::semantic_type* yylval, myFlexLexer &lexer) {
    int tok = lexer.yylex();

    #ifndef _LEXER_STANDALONE_
    /*parser::symbol_type &symbol = lexer.symbol;
    if (tok == parser::token::ID) 
      parser::make_ID(lexer.YYText()).move(symbol);
    else if (tok == parser::token::INT) 
      parser::make_INT(atoi(lexer.YYText())).move(symbol);
    */
    yylval = new parser::semantic_type();
    #endif /* !_LEXER_STANDALONE_ */

    return tok;
  }

  void parser::error(const std::string &msg) {
    std::cerr << msg << "\n";
  }
%}

  /***********************/
  /* BISON DECLARATIONS: */
  /***********************/

%token KW_VAR
%token KW_TYPE
%token COLON
%token LPAREN
%token RPAREN
%token LBRACK
%token RBRACK
%token STAR
%token DOT
%token EQUAL
%token <int> INT
%token <std::string> ID

%start program

%type <Program *> program
%type <DeclList *> decl_list
%type <Decl *> decl var_decl type_decl
%type <StmtList *> stmt_list
%type <Stmt *> stmt
%type <Expr *> type_expr expr
%type <Factor *> factor
%type <Identifier *> identifier
%type <Integer *> integer
%type <BrackExpr *> brack_expr
%type <ParenExpr *> paren_expr
%type <ExprList *> expr_list

%%

  /******************/
  /* GRAMMAR RULES: */
  /******************/

program : decl_list stmt_list { root = new Program($1, $2); }

decl_list : decl_list decl { $$ = $1->append($2); }
          | decl  { $$ = new DeclList($1); }

decl : var_decl { $$ = $1; }
     | type_decl { $$ = $1; }

var_decl : KW_VAR identifier COLON type_expr { $$ = new Decl(NT_VarDecl, $2, $4); }

type_decl : KW_TYPE identifier COLON type_expr { $$ = new Decl(NT_TypeDecl, $2, $4); }

stmt_list : stmt_list stmt { $$ = $1->append($2); }
          | stmt { $$ = new StmtList($1); }

stmt : identifier EQUAL expr { $$ = new Stmt($1, $3); }

type_expr : expr { $$ = $1; }

expr : expr STAR factor { $$ = new BinaryExpr(NT_TensorExpr, $1, $3); }
     | expr DOT factor { $$ = new BinaryExpr(NT_DotExpr, $1, $3); }
     | factor { $$ = $1; }

factor : identifier { $$ = $1; }
       | integer { $$ = $1; }
       | brack_expr { $$ = $1; }
       | paren_expr { $$ = $1; }

identifier : ID { $$ = new Identifier($1); }

integer : INT { $$ = new Integer($1); }
     
brack_expr : LBRACK expr_list RBRACK { $$ = new BrackExpr($2); }
       
paren_expr : LPAREN expr RPAREN { $$ = new ParenExpr($2); }

expr_list : /* empty */ { $$ = new ExprList(); }
          | expr_list expr { $$ = $1->append($2); }

%%

  /**************/
  /* USER CODE: */
  /**************/

#ifdef _PARSER_STANDALONE_

int main(int argc, char* argv[]) {
  const Program *root;
  myFlexLexer lexer;

  std::ifstream in;

  if (argc != 2) {
    return 1;
  } else {
    in.open(argv[1]);
    if (!in.is_open())
      return 2;
  }

  lexer.switch_streams(&in);
  parser(lexer, root).parse();

  in.close();

  root->print();
  root->deepDelete();

  return 0;
}

#endif /* _PARSER_STANDALONE_ */

