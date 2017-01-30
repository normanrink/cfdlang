// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.


// First part of user declarations.
#line 33 "lang.yy" // lalr1.cc:404

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

#line 72 "lang.tab.cc" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "lang.tab.hh"

// User implementation prologue.

#line 86 "lang.tab.cc" // lalr1.cc:412


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif



// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace yy {
#line 153 "lang.tab.cc" // lalr1.cc:479

  /// Build a parser object.
  parser::parser (myFlexLexer &lexer_yyarg, const Program *&root_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      lexer (lexer_yyarg),
      root (root_yyarg)
  {}

  parser::~parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  parser::syntax_error::syntax_error (const std::string& m)
    : std::runtime_error (m)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  parser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  parser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
  {
      switch (other.type_get ())
    {
      case 28: // brack_expr
        value.copy< BrackExpr * > (other.value);
        break;

      case 18: // decl
      case 19: // var_decl
      case 20: // type_decl
        value.copy< Decl * > (other.value);
        break;

      case 17: // decl_list
        value.copy< DeclList * > (other.value);
        break;

      case 23: // type_expr
      case 24: // expr
        value.copy< Expr * > (other.value);
        break;

      case 30: // expr_list
        value.copy< ExprList * > (other.value);
        break;

      case 25: // factor
        value.copy< Factor * > (other.value);
        break;

      case 26: // identifier
        value.copy< Identifier * > (other.value);
        break;

      case 27: // integer
        value.copy< Integer * > (other.value);
        break;

      case 29: // paren_expr
        value.copy< ParenExpr * > (other.value);
        break;

      case 16: // program
        value.copy< Program * > (other.value);
        break;

      case 22: // stmt
        value.copy< Stmt * > (other.value);
        break;

      case 21: // stmt_list
        value.copy< StmtList * > (other.value);
        break;

      case 13: // INT
        value.copy< int > (other.value);
        break;

      case 14: // ID
        value.copy< std::string > (other.value);
        break;

      default:
        break;
    }

  }


  template <typename Base>
  inline
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v)
    : Base (t)
    , value ()
  {
    (void) v;
      switch (this->type_get ())
    {
      case 28: // brack_expr
        value.copy< BrackExpr * > (v);
        break;

      case 18: // decl
      case 19: // var_decl
      case 20: // type_decl
        value.copy< Decl * > (v);
        break;

      case 17: // decl_list
        value.copy< DeclList * > (v);
        break;

      case 23: // type_expr
      case 24: // expr
        value.copy< Expr * > (v);
        break;

      case 30: // expr_list
        value.copy< ExprList * > (v);
        break;

      case 25: // factor
        value.copy< Factor * > (v);
        break;

      case 26: // identifier
        value.copy< Identifier * > (v);
        break;

      case 27: // integer
        value.copy< Integer * > (v);
        break;

      case 29: // paren_expr
        value.copy< ParenExpr * > (v);
        break;

      case 16: // program
        value.copy< Program * > (v);
        break;

      case 22: // stmt
        value.copy< Stmt * > (v);
        break;

      case 21: // stmt_list
        value.copy< StmtList * > (v);
        break;

      case 13: // INT
        value.copy< int > (v);
        break;

      case 14: // ID
        value.copy< std::string > (v);
        break;

      default:
        break;
    }
}


  // Implementation of basic_symbol constructor for each type.

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t)
    : Base (t)
    , value ()
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const BrackExpr * v)
    : Base (t)
    , value (v)
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const Decl * v)
    : Base (t)
    , value (v)
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const DeclList * v)
    : Base (t)
    , value (v)
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const Expr * v)
    : Base (t)
    , value (v)
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ExprList * v)
    : Base (t)
    , value (v)
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const Factor * v)
    : Base (t)
    , value (v)
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const Identifier * v)
    : Base (t)
    , value (v)
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const Integer * v)
    : Base (t)
    , value (v)
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ParenExpr * v)
    : Base (t)
    , value (v)
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const Program * v)
    : Base (t)
    , value (v)
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const Stmt * v)
    : Base (t)
    , value (v)
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const StmtList * v)
    : Base (t)
    , value (v)
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const int v)
    : Base (t)
    , value (v)
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::string v)
    : Base (t)
    , value (v)
  {}


  template <typename Base>
  inline
  parser::basic_symbol<Base>::~basic_symbol ()
  {
    clear ();
  }

  template <typename Base>
  inline
  void
  parser::basic_symbol<Base>::clear ()
  {
    // User destructor.
    symbol_number_type yytype = this->type_get ();
    basic_symbol<Base>& yysym = *this;
    (void) yysym;
    switch (yytype)
    {
   default:
      break;
    }

    // Type destructor.
    switch (yytype)
    {
      case 28: // brack_expr
        value.template destroy< BrackExpr * > ();
        break;

      case 18: // decl
      case 19: // var_decl
      case 20: // type_decl
        value.template destroy< Decl * > ();
        break;

      case 17: // decl_list
        value.template destroy< DeclList * > ();
        break;

      case 23: // type_expr
      case 24: // expr
        value.template destroy< Expr * > ();
        break;

      case 30: // expr_list
        value.template destroy< ExprList * > ();
        break;

      case 25: // factor
        value.template destroy< Factor * > ();
        break;

      case 26: // identifier
        value.template destroy< Identifier * > ();
        break;

      case 27: // integer
        value.template destroy< Integer * > ();
        break;

      case 29: // paren_expr
        value.template destroy< ParenExpr * > ();
        break;

      case 16: // program
        value.template destroy< Program * > ();
        break;

      case 22: // stmt
        value.template destroy< Stmt * > ();
        break;

      case 21: // stmt_list
        value.template destroy< StmtList * > ();
        break;

      case 13: // INT
        value.template destroy< int > ();
        break;

      case 14: // ID
        value.template destroy< std::string > ();
        break;

      default:
        break;
    }

    Base::clear ();
  }

  template <typename Base>
  inline
  bool
  parser::basic_symbol<Base>::empty () const
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  inline
  void
  parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
      switch (this->type_get ())
    {
      case 28: // brack_expr
        value.move< BrackExpr * > (s.value);
        break;

      case 18: // decl
      case 19: // var_decl
      case 20: // type_decl
        value.move< Decl * > (s.value);
        break;

      case 17: // decl_list
        value.move< DeclList * > (s.value);
        break;

      case 23: // type_expr
      case 24: // expr
        value.move< Expr * > (s.value);
        break;

      case 30: // expr_list
        value.move< ExprList * > (s.value);
        break;

      case 25: // factor
        value.move< Factor * > (s.value);
        break;

      case 26: // identifier
        value.move< Identifier * > (s.value);
        break;

      case 27: // integer
        value.move< Integer * > (s.value);
        break;

      case 29: // paren_expr
        value.move< ParenExpr * > (s.value);
        break;

      case 16: // program
        value.move< Program * > (s.value);
        break;

      case 22: // stmt
        value.move< Stmt * > (s.value);
        break;

      case 21: // stmt_list
        value.move< StmtList * > (s.value);
        break;

      case 13: // INT
        value.move< int > (s.value);
        break;

      case 14: // ID
        value.move< std::string > (s.value);
        break;

      default:
        break;
    }

  }

  // by_type.
  inline
  parser::by_type::by_type ()
    : type (empty_symbol)
  {}

  inline
  parser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  parser::by_type::clear ()
  {
    type = empty_symbol;
  }

  inline
  void
  parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  inline
  int
  parser::by_type::type_get () const
  {
    return type;
  }
  // Implementation of make_symbol for each symbol type.
  parser::symbol_type
  parser::make_KW_VAR ()
  {
    return symbol_type (token::KW_VAR);
  }

  parser::symbol_type
  parser::make_KW_TYPE ()
  {
    return symbol_type (token::KW_TYPE);
  }

  parser::symbol_type
  parser::make_COLON ()
  {
    return symbol_type (token::COLON);
  }

  parser::symbol_type
  parser::make_LPAREN ()
  {
    return symbol_type (token::LPAREN);
  }

  parser::symbol_type
  parser::make_RPAREN ()
  {
    return symbol_type (token::RPAREN);
  }

  parser::symbol_type
  parser::make_LBRACK ()
  {
    return symbol_type (token::LBRACK);
  }

  parser::symbol_type
  parser::make_RBRACK ()
  {
    return symbol_type (token::RBRACK);
  }

  parser::symbol_type
  parser::make_STAR ()
  {
    return symbol_type (token::STAR);
  }

  parser::symbol_type
  parser::make_DOT ()
  {
    return symbol_type (token::DOT);
  }

  parser::symbol_type
  parser::make_EQUAL ()
  {
    return symbol_type (token::EQUAL);
  }

  parser::symbol_type
  parser::make_INT (const int& v)
  {
    return symbol_type (token::INT, v);
  }

  parser::symbol_type
  parser::make_ID (const std::string& v)
  {
    return symbol_type (token::ID, v);
  }



  // by_state.
  inline
  parser::by_state::by_state ()
    : state (empty_state)
  {}

  inline
  parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  parser::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
  parser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  parser::symbol_number_type
  parser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
  parser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  parser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s)
  {
      switch (that.type_get ())
    {
      case 28: // brack_expr
        value.move< BrackExpr * > (that.value);
        break;

      case 18: // decl
      case 19: // var_decl
      case 20: // type_decl
        value.move< Decl * > (that.value);
        break;

      case 17: // decl_list
        value.move< DeclList * > (that.value);
        break;

      case 23: // type_expr
      case 24: // expr
        value.move< Expr * > (that.value);
        break;

      case 30: // expr_list
        value.move< ExprList * > (that.value);
        break;

      case 25: // factor
        value.move< Factor * > (that.value);
        break;

      case 26: // identifier
        value.move< Identifier * > (that.value);
        break;

      case 27: // integer
        value.move< Integer * > (that.value);
        break;

      case 29: // paren_expr
        value.move< ParenExpr * > (that.value);
        break;

      case 16: // program
        value.move< Program * > (that.value);
        break;

      case 22: // stmt
        value.move< Stmt * > (that.value);
        break;

      case 21: // stmt_list
        value.move< StmtList * > (that.value);
        break;

      case 13: // INT
        value.move< int > (that.value);
        break;

      case 14: // ID
        value.move< std::string > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty_symbol;
  }

  inline
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
      switch (that.type_get ())
    {
      case 28: // brack_expr
        value.copy< BrackExpr * > (that.value);
        break;

      case 18: // decl
      case 19: // var_decl
      case 20: // type_decl
        value.copy< Decl * > (that.value);
        break;

      case 17: // decl_list
        value.copy< DeclList * > (that.value);
        break;

      case 23: // type_expr
      case 24: // expr
        value.copy< Expr * > (that.value);
        break;

      case 30: // expr_list
        value.copy< ExprList * > (that.value);
        break;

      case 25: // factor
        value.copy< Factor * > (that.value);
        break;

      case 26: // identifier
        value.copy< Identifier * > (that.value);
        break;

      case 27: // integer
        value.copy< Integer * > (that.value);
        break;

      case 29: // paren_expr
        value.copy< ParenExpr * > (that.value);
        break;

      case 16: // program
        value.copy< Program * > (that.value);
        break;

      case 22: // stmt
        value.copy< Stmt * > (that.value);
        break;

      case 21: // stmt_list
        value.copy< StmtList * > (that.value);
        break;

      case 13: // INT
        value.copy< int > (that.value);
        break;

      case 14: // ID
        value.copy< std::string > (that.value);
        break;

      default:
        break;
    }

    return *this;
  }


  template <typename Base>
  inline
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " (";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  parser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  parser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  parser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, lexer));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
        switch (yyr1_[yyn])
    {
      case 28: // brack_expr
        yylhs.value.build< BrackExpr * > ();
        break;

      case 18: // decl
      case 19: // var_decl
      case 20: // type_decl
        yylhs.value.build< Decl * > ();
        break;

      case 17: // decl_list
        yylhs.value.build< DeclList * > ();
        break;

      case 23: // type_expr
      case 24: // expr
        yylhs.value.build< Expr * > ();
        break;

      case 30: // expr_list
        yylhs.value.build< ExprList * > ();
        break;

      case 25: // factor
        yylhs.value.build< Factor * > ();
        break;

      case 26: // identifier
        yylhs.value.build< Identifier * > ();
        break;

      case 27: // integer
        yylhs.value.build< Integer * > ();
        break;

      case 29: // paren_expr
        yylhs.value.build< ParenExpr * > ();
        break;

      case 16: // program
        yylhs.value.build< Program * > ();
        break;

      case 22: // stmt
        yylhs.value.build< Stmt * > ();
        break;

      case 21: // stmt_list
        yylhs.value.build< StmtList * > ();
        break;

      case 13: // INT
        yylhs.value.build< int > ();
        break;

      case 14: // ID
        yylhs.value.build< std::string > ();
        break;

      default:
        break;
    }



      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 2:
#line 107 "lang.yy" // lalr1.cc:859
    { root = new Program(yystack_[1].value.as< DeclList * > (), yystack_[0].value.as< StmtList * > ()); }
#line 1192 "lang.tab.cc" // lalr1.cc:859
    break;

  case 3:
#line 109 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< DeclList * > () = yystack_[1].value.as< DeclList * > ()->append(yystack_[0].value.as< Decl * > ()); }
#line 1198 "lang.tab.cc" // lalr1.cc:859
    break;

  case 4:
#line 110 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< DeclList * > () = new DeclList(yystack_[0].value.as< Decl * > ()); }
#line 1204 "lang.tab.cc" // lalr1.cc:859
    break;

  case 5:
#line 112 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< Decl * > () = yystack_[0].value.as< Decl * > (); }
#line 1210 "lang.tab.cc" // lalr1.cc:859
    break;

  case 6:
#line 113 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< Decl * > () = yystack_[0].value.as< Decl * > (); }
#line 1216 "lang.tab.cc" // lalr1.cc:859
    break;

  case 7:
#line 115 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< Decl * > () = new Decl(NT_VarDecl, yystack_[2].value.as< Identifier * > (), yystack_[0].value.as< Expr * > ()); }
#line 1222 "lang.tab.cc" // lalr1.cc:859
    break;

  case 8:
#line 117 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< Decl * > () = new Decl(NT_TypeDecl, yystack_[2].value.as< Identifier * > (), yystack_[0].value.as< Expr * > ()); }
#line 1228 "lang.tab.cc" // lalr1.cc:859
    break;

  case 9:
#line 119 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< StmtList * > () = yystack_[1].value.as< StmtList * > ()->append(yystack_[0].value.as< Stmt * > ()); }
#line 1234 "lang.tab.cc" // lalr1.cc:859
    break;

  case 10:
#line 120 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< StmtList * > () = new StmtList(yystack_[0].value.as< Stmt * > ()); }
#line 1240 "lang.tab.cc" // lalr1.cc:859
    break;

  case 11:
#line 122 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< Stmt * > () = new Stmt(yystack_[2].value.as< Identifier * > (), yystack_[0].value.as< Expr * > ()); }
#line 1246 "lang.tab.cc" // lalr1.cc:859
    break;

  case 12:
#line 124 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< Expr * > () = yystack_[0].value.as< Expr * > (); }
#line 1252 "lang.tab.cc" // lalr1.cc:859
    break;

  case 13:
#line 126 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< Expr * > () = new BinaryExpr(NT_TensorExpr, yystack_[2].value.as< Expr * > (), yystack_[0].value.as< Factor * > ()); }
#line 1258 "lang.tab.cc" // lalr1.cc:859
    break;

  case 14:
#line 127 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< Expr * > () = new BinaryExpr(NT_DotExpr, yystack_[2].value.as< Expr * > (), yystack_[0].value.as< Factor * > ()); }
#line 1264 "lang.tab.cc" // lalr1.cc:859
    break;

  case 15:
#line 128 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< Expr * > () = yystack_[0].value.as< Factor * > (); }
#line 1270 "lang.tab.cc" // lalr1.cc:859
    break;

  case 16:
#line 130 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< Factor * > () = yystack_[0].value.as< Identifier * > (); }
#line 1276 "lang.tab.cc" // lalr1.cc:859
    break;

  case 17:
#line 131 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< Factor * > () = yystack_[0].value.as< Integer * > (); }
#line 1282 "lang.tab.cc" // lalr1.cc:859
    break;

  case 18:
#line 132 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< Factor * > () = yystack_[0].value.as< BrackExpr * > (); }
#line 1288 "lang.tab.cc" // lalr1.cc:859
    break;

  case 19:
#line 133 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< Factor * > () = yystack_[0].value.as< ParenExpr * > (); }
#line 1294 "lang.tab.cc" // lalr1.cc:859
    break;

  case 20:
#line 135 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< Identifier * > () = new Identifier(yystack_[0].value.as< std::string > ()); }
#line 1300 "lang.tab.cc" // lalr1.cc:859
    break;

  case 21:
#line 137 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< Integer * > () = new Integer(yystack_[0].value.as< int > ()); }
#line 1306 "lang.tab.cc" // lalr1.cc:859
    break;

  case 22:
#line 139 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< BrackExpr * > () = new BrackExpr(yystack_[1].value.as< ExprList * > ()); }
#line 1312 "lang.tab.cc" // lalr1.cc:859
    break;

  case 23:
#line 141 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< ParenExpr * > () = new ParenExpr(yystack_[1].value.as< Expr * > ()); }
#line 1318 "lang.tab.cc" // lalr1.cc:859
    break;

  case 24:
#line 143 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< ExprList * > () = new ExprList(); }
#line 1324 "lang.tab.cc" // lalr1.cc:859
    break;

  case 25:
#line 144 "lang.yy" // lalr1.cc:859
    { yylhs.value.as< ExprList * > () = yystack_[1].value.as< ExprList * > ()->append(yystack_[0].value.as< Expr * > ()); }
#line 1330 "lang.tab.cc" // lalr1.cc:859
    break;


#line 1334 "lang.tab.cc" // lalr1.cc:859
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yysyntax_error_ (yystack_[0].state, yyla));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }


      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.what());
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (state_type, const symbol_type&) const
  {
    return YY_("syntax error");
  }


  const signed char parser::yypact_ninf_ = -20;

  const signed char parser::yytable_ninf_ = -1;

  const signed char
  parser::yypact_[] =
  {
      19,   -12,   -12,     5,     4,   -20,   -20,   -20,   -20,     8,
      15,   -20,   -20,   -12,   -20,    12,    13,    13,   -20,    13,
      13,   -20,   -20,   -20,    20,   -20,   -20,   -20,   -20,   -20,
     -20,    20,    18,     3,    13,    13,   -20,   -20,    20,   -20,
     -20
  };

  const unsigned char
  parser::yydefact_[] =
  {
       0,     0,     0,     0,     0,     4,     5,     6,    20,     0,
       0,     1,     3,     2,    10,     0,     0,     0,     9,     0,
       0,    24,    21,     7,    12,    15,    16,    17,    18,    19,
       8,    11,     0,     0,     0,     0,    23,    22,    25,    13,
      14
  };

  const signed char
  parser::yypgoto_[] =
  {
     -20,   -20,   -20,     6,   -20,   -20,   -20,    21,    22,   -19,
      -2,     2,   -20,   -20,   -20,   -20
  };

  const signed char
  parser::yydefgoto_[] =
  {
      -1,     3,     4,     5,     6,     7,    13,    14,    23,    24,
      25,    26,    27,    28,    29,    33
  };

  const unsigned char
  parser::yytable_[] =
  {
      31,    32,     8,     9,    10,    11,    15,     1,     2,    20,
      12,    21,    37,    16,    38,    15,    22,     8,     8,    20,
      17,    21,     1,     2,    19,    36,    22,     8,    34,    35,
      34,    35,    39,    40,    18,     0,     0,     0,     0,    30
  };

  const signed char
  parser::yycheck_[] =
  {
      19,    20,    14,     1,     2,     0,     4,     3,     4,     6,
       4,     8,     9,     5,    33,    13,    13,    14,    14,     6,
       5,     8,     3,     4,    12,     7,    13,    14,    10,    11,
      10,    11,    34,    35,    13,    -1,    -1,    -1,    -1,    17
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,     3,     4,    16,    17,    18,    19,    20,    14,    26,
      26,     0,    18,    21,    22,    26,     5,     5,    22,    12,
       6,     8,    13,    23,    24,    25,    26,    27,    28,    29,
      23,    24,    24,    30,    10,    11,     7,     9,    24,    25,
      25
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    15,    16,    17,    17,    18,    18,    19,    20,    21,
      21,    22,    23,    24,    24,    24,    25,    25,    25,    25,
      26,    27,    28,    29,    30,    30
  };

  const unsigned char
  parser::yyr2_[] =
  {
       0,     2,     2,     2,     1,     1,     1,     4,     4,     2,
       1,     3,     1,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     0,     2
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "$end", "error", "$undefined", "KW_VAR", "KW_TYPE", "COLON", "LPAREN",
  "RPAREN", "LBRACK", "RBRACK", "STAR", "DOT", "EQUAL", "INT", "ID",
  "$accept", "program", "decl_list", "decl", "var_decl", "type_decl",
  "stmt_list", "stmt", "type_expr", "expr", "factor", "identifier",
  "integer", "brack_expr", "paren_expr", "expr_list", YY_NULLPTR
  };


  const unsigned char
  parser::yyrline_[] =
  {
       0,   107,   107,   109,   110,   112,   113,   115,   117,   119,
     120,   122,   124,   126,   127,   128,   130,   131,   132,   133,
     135,   137,   139,   141,   143,   144
  };

  // Print the state stack on the debug stream.
  void
  parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  inline
  parser::token_number_type
  parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14
    };
    const unsigned int user_token_number_max_ = 269;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }


} // yy
#line 1679 "lang.tab.cc" // lalr1.cc:1167
#line 146 "lang.yy" // lalr1.cc:1168


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

