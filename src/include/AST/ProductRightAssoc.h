
#ifndef __PRODUCT_RIGHT_ASSOC_H__
#define __PRODUCT_RIGHT_ASSOC_H__

#include <list>
#include <map>
#include <set>
#include <vector>


#include "AST/AST.h"


namespace CFDlang {

class ProductRightAssoc : public ASTVisitor {
private:
  const ASTNode *curResult;

public:
  const Program *run(const Program *p);

  virtual void visitProgram(const Program *p) override;
  virtual void visitStmtList(const StmtList *l) override;

  virtual void visitDecl(const Decl *) override {}

  virtual void visitElemDirect(const ElemDirect *) override {};

  virtual void visitStmt(const Stmt *s) override;

  virtual void visitBinaryExpr(const BinaryExpr *be) override;
  virtual void visitParenExpr(const ParenExpr *pe) override;

  virtual void visitIdentifier(const Identifier *id) override {
    curResult = id;
  }
  virtual void visitInteger(const Integer *i) override {
    curResult = i;
  }
  virtual void visitBrackExpr(const BrackExpr *be) override {
    curResult = be;
  }
};

};

#endif /* PRODUCT_RIGHT_ASSOC */
