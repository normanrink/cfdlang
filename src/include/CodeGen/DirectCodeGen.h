
#ifndef __DIRECT_CODEGEN_H__
#define __DIRECT_CODEGEN_H__

#include <map>
#include <string>


#include "AST/AST.h"
#include "CodeGen/CodeGen.h"


class DirectCodeGen : public CodeGen {
  // map 'Expr' nodes in the AST to temporary variables:
  std::map<const Expr *, std::string> ExprTemps;

protected:
  const std::string addTempForExpr(const Expr *e);
  const std::string addNameForExpr(const Expr *e, const std::string &name);
  const std::string getTempForExpr(const Expr *e) const;

public:
  DirectCodeGen(const Sema *sema);

  virtual void visitProgram(const Program *p) override;

  virtual void visitDecl(const Decl *d) override;
  virtual void visitStmt(const Stmt *s) override;

  virtual void visitBinaryExpr(const BinaryExpr *be) override;
  virtual void visitIdentifier(const Identifier *id) override;
  virtual void visitInteger(const Integer *i) override;
  virtual void visitBrackExpr(const BrackExpr *be) override;
  virtual void visitParenExpr(const ParenExpr *pe) override;

  virtual const std::string visitContraction(const Expr *e,
                                             const TupleList &indices);

  virtual void visitProgramPrologue(const Program *p) {}
  virtual void visitProgramEpilogue(const Program *p) {}
  
  virtual void visitDeclPrologue(const Decl *d) {}
  virtual void visitDeclEpilogue(const Decl *d) {}
  virtual void visitStmtPrologue(const Stmt *d) {}
  virtual void visitStmtEpilogue(const Stmt *d) {}
  
  virtual void visitContractionPrologue(const Expr *e,
                                        const TupleList &indices) {}
  virtual const std::string
  visitContractionEpilogue(const Expr *e,
                           const std::string &lhs, const std::string &rhs,
                           const TupleList &LeftAndRightInds) {}

  virtual void visitBinaryExprPrologue(const BinaryExpr *be) {}
  virtual void visitBinaryExprEpilogue(const BinaryExpr *be) {}
  virtual void visitTensorExprPrologue(const BinaryExpr *be) {}
  virtual void visitTensorExprEpilogue(const BinaryExpr *be) {}
  virtual void visitDotExprPrologue(const BinaryExpr *be) {}
  virtual void visitDotExprEpilogue(const BinaryExpr *be) {}
  virtual void visitIdentifierPrologue(const Identifier *id) {}
  virtual void visitIdentifierEpilogue(const Identifier *id) {}
  virtual void visitIntegerPrologue(const Integer *i) {}
  virtual void visitIntegerEpilogue(const Integer *i) {}
  virtual void visitBrackExprPrologue(const BrackExpr *be) {}
  virtual void visitBrackExprEpilogue(const BrackExpr *be) {}
  virtual void visitParenExprPrologue(const ParenExpr *pe) {}
  virtual void visitParenExprEpilogue(const ParenExpr *pe) {}
};

#endif /* __DIRECT_CODEGEN_H__ */

