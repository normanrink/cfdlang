
#ifndef __DIRECT_CODEGEN_H__
#define __DIRECT_CODEGEN_H__

#include <map>
#include <string>


#include "AST/AST.h"
#include "CodeGen/CodeGen.h"
#include "CodeGen/ExprTree.h"


namespace CFDlang {

class DirectCodeGen : public CodeGen {
public:
  DirectCodeGen(const Sema *sema, const std::string &functionName);

  virtual void visitStmt(const Stmt *s) override;

  virtual void visitElemDirect(const ElemDirect *ed) override {}

  virtual void visitBinaryExpr(const BinaryExpr *be) override;
  virtual void visitIdentifier(const Identifier *id) override;
  virtual void visitInteger(const Integer *i) override {}
  virtual void visitBrackExpr(const BrackExpr *be) override;
  virtual void visitParenExpr(const ParenExpr *pe) override;

  void visitContraction(const Expr *e, const TupleList &indices);
};

};

#endif /* __DIRECT_CODEGEN_H__ */
