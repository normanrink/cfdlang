
#include <string>
#include <cassert>

#include "ast_c.h"
#include "ast.h"



/****************/
/* expressions: */
/****************/

static NODE_PTR createBinaryExpr(NodeType nt, const NODE_PTR left, const NODE_PTR right) {
  const Expr *left_cast = dynamic_cast<const Expr *>((const Node *)left);
  assert(left_cast);

  const Factor *right_cast = dynamic_cast<const Factor *>((const Node *)right);
  assert(right_cast);

  return (NODE_PTR) BinaryExpr::create(nt, left_cast, right_cast);
}

NODE_PTR createTensorExpr(const NODE_PTR left, const NODE_PTR right) {
  return createBinaryExpr(NT_TensorExpr, left, right);
}

NODE_PTR createDotExpr(const NODE_PTR left, const NODE_PTR right) {
  return createBinaryExpr(NT_DotExpr, left, right);
}


NODE_PTR createIdentifier(const char *name) {
 return (NODE_PTR) Identifier::create(std::string(name)); 
}


NODE_PTR createInteger(int value) {
 return (NODE_PTR) Integer::create(value); 
}


NODE_PTR createBrackExpr(const NODE_PTR list) {
  const ExprList *list_cast = dynamic_cast<const ExprList *>((const Node *)list);
  assert(list_cast);

  return (NODE_PTR) BrackExpr::create(list_cast);
}


NODE_PTR createParenExpr(const NODE_PTR expr) {
  const Expr *expr_cast = dynamic_cast<const Expr *>((const Node *)expr);
  assert(expr_cast);

  return (NODE_PTR) ParenExpr::create(expr_cast);
}



/**************/
/* statement: */
/**************/

NODE_PTR createStmt(const NODE_PTR id, const NODE_PTR expr) {
  const Identifier *id_cast = dynamic_cast<const Identifier *>((const Node *)id);
  assert(id_cast);

  const Expr *expr_cast = dynamic_cast<const Expr *>((const Node *)expr);
  assert(expr_cast);

  return (NODE_PTR) Stmt::create(id_cast, expr_cast);
}



/****************/
/* declaration: */
/****************/

static NODE_PTR createDecl(NodeType nt, const NODE_PTR id, const NODE_PTR expr) {
  const Identifier *id_cast = dynamic_cast<const Identifier *>((const Node *)id);
  assert(id_cast);

  const Expr *expr_cast = dynamic_cast<const Expr *>((const Node *)expr);
  assert(expr_cast);

  return (NODE_PTR) Decl::create(nt, id_cast, expr_cast);
}

NODE_PTR createVarDecl(const NODE_PTR id, const NODE_PTR expr) {
  return createDecl(NT_VarDecl, id, expr);
}

NODE_PTR createTypeDecl(const NODE_PTR id, const NODE_PTR expr) {
  return createDecl(NT_TypeDecl, id, expr);
}



/*************/
/* program:: */
/*************/

NODE_PTR createProgram(const NODE_PTR decls, const NODE_PTR stmts) {
  const DeclList *decls_cast = dynamic_cast<const DeclList *>((const Node *)decls);
  assert(decls_cast);

  const StmtList *stmts_cast = dynamic_cast<const StmtList *>((const Node *)stmts);
  assert(stmts_cast);

  return (NODE_PTR) Program::create(decls_cast, stmts_cast);
}



/**********/
/* lists: */
/**********/

NODE_PTR createDeclList(const NODE_PTR d) {
  const Decl *d_cast = dynamic_cast<const Decl *>((const Node *)d);
  assert(d_cast);

  return (NODE_PTR) DeclList::create(d_cast);
}

NODE_PTR appendDeclList(NODE_PTR list, const NODE_PTR d) {
  DeclList *list_cast = dynamic_cast<DeclList *>((Node *)list);
  assert(list_cast);

  const Decl *d_cast = dynamic_cast<const Decl *>((const Node *)d);
  assert(d_cast);

  return (NODE_PTR) DeclList::append(list_cast, d_cast);
}


NODE_PTR createStmtList(const NODE_PTR s) {
  const Stmt *s_cast = dynamic_cast<const Stmt *>((const Node *)s);
  assert(s_cast);

  return (NODE_PTR) StmtList::create(s_cast);
}

NODE_PTR appendStmtList(NODE_PTR list, const NODE_PTR s) {
  StmtList *list_cast = dynamic_cast<StmtList *>((Node *)list);
  assert(list_cast);

  const Stmt *s_cast = dynamic_cast<const Stmt *>((const Node *)s);
  assert(s_cast);

  return (NODE_PTR) StmtList::append(list_cast, s_cast);
}


NODE_PTR createExprList() {
  return (NODE_PTR) ExprList::create();
}

NODE_PTR appendExprList(NODE_PTR list, const NODE_PTR e) {
  ExprList *list_cast = dynamic_cast<ExprList *>((Node *)list);
  assert(list_cast);

  const Expr *e_cast = dynamic_cast<const Expr *>((const Node *)e);
  assert(e_cast);

  return (NODE_PTR) ExprList::append(list_cast, e_cast);
}



void printNode(const NODE_PTR n) {
  ((const Node *)n)->print();
}

void printAST(const NODE_PTR p) {
  const Program *p_cast = dynamic_cast<const Program *>((const Node *)p);
  assert(p_cast);
  
  p_cast->print();
}

void destroyAST(const NODE_PTR p) {
  const Program *p_cast = dynamic_cast<const Program *>((const Node *)p);
  assert(p_cast);

  Program::destroy(p_cast);
}

