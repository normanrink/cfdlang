
#include <assert.h>
#include <list>
#include <set>
#include <string>
#include <vector>


#include "AST/ProductRightAssoc.h"


using namespace CFDlang;


const Program *ProductRightAssoc::run(const Program *p) {
  p->visit(this);
  const Program *newP = dynamic_cast<const Program *>(curResult);
  return newP;
}

void ProductRightAssoc::visitProgram(const Program *p) {
  p->getStmts()->visit(this);
  const StmtList *l = dynamic_cast<const StmtList *>(curResult);
  curResult = Program::create(p->getDecls(), l);
}

void ProductRightAssoc::visitStmtList(const StmtList *l) {
  StmtList *result = StmtList::create();

  for (const auto *stmt: *l) {
      stmt->visit(this);
      const Stmt *newStmt  = dynamic_cast<const Stmt *>(curResult);
      result->append(newStmt);
  }
  curResult = result;
}

void ProductRightAssoc::visitStmt(const Stmt *s) {
  s->getExpr()->visit(this);
  const Expr *expr = dynamic_cast<const Expr *>(curResult);
  curResult = Stmt::create(s->getIdentifier(), expr);
}

void ProductRightAssoc::visitBinaryExpr(const BinaryExpr *be) {
  be->getLeft()->visit(this);
  const Expr *left = dynamic_cast<const Expr *>(curResult);

  be->getRight()->visit(this);
  const Expr *right = dynamic_cast<const Expr *>(curResult);

  if ((be->getNodeType() == ASTNode::NT_ProductExpr)
      && (left->getNodeType() == ASTNode::NT_ProductExpr)) {
    const BinaryExpr *leftBE = dynamic_cast<const BinaryExpr *>(left);

    const BinaryExpr *newRootBE =
      BinaryExpr::create(ASTNode::NT_ProductExpr,
                         leftBE->getLeft(),
                         BinaryExpr::create(ASTNode::NT_ProductExpr,
                                            leftBE->getRight(),
                                            right));
    newRootBE->visit(this);
    return;
  }

  curResult = CFDlang::BinaryExpr::create(be->getNodeType(), left, right);
}

void ProductRightAssoc::visitParenExpr(const ParenExpr *pe) {
  pe->getExpr()->visit(this);
  const Expr *expr = dynamic_cast<const Expr *>(curResult);
  curResult = CFDlang::ParenExpr::create(expr);
}
