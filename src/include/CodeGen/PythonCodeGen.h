
#ifndef __PYTHON_CODEGEN_H__
#define __PYTHON_CODEGEN_H__

#include <string>


#include "CodeGen/DirectCodeGen.h"
#include "CodeGen/GraphCodeGen.h"
#include "CodeGen/PythonFragmentBuilder.h"


class NumpyDirectCG : public DirectCodeGen {
protected:
  PythonFragBuilder Builder;

public:
  NumpyDirectCG(const Sema *sema, const std::string &prefix = "np");

  virtual void visitProgramPrologue(const Program *p) override;

  virtual void visitDeclEpilogue(const Decl *d) override;
  virtual void visitStmtEpilogue(const Stmt *s) override;

  virtual const std::string
  visitContractionEpilogue(const Expr *e,
                           const std::string &lhs, const std::string &rhs,
                           const TupleList &LeftAndRightIndices);

  virtual void visitAddExprEpilogue(const BinaryExpr *be) override;
  virtual void visitSubExprEpilogue(const BinaryExpr *be) override;
  virtual void visitMulExprEpilogue(const BinaryExpr *be) override;
  virtual void visitDivExprEpilogue(const BinaryExpr *be) override;
  virtual void visitProductExprEpilogue(const BinaryExpr *be) override;

  virtual void visitBrackExprEpilogue(const BrackExpr *be) override;
};


class TheanoDirectCG : public NumpyDirectCG {
public:
  TheanoDirectCG(const Sema *sema, const std::string &prefix = "T");

  virtual void visitProgramPrologue(const Program *p) override;
  virtual void visitProgramEpilogue(const Program *p) override;

  virtual void visitDeclEpilogue(const Decl *d) override;
};


class NumpyGraphCG : public GraphCodeGen {
protected:
  PythonFragBuilder Builder;

public:
  NumpyGraphCG(const Sema *sema, const std::string &prefix = "np");

  virtual void visitProgramPrologue(const Program *p) override;

  virtual void visitDeclEpilogue(const Decl *d) override;

  virtual void
  emitContraction(const std::string &result,
                  const std::string &lhs, const List &lhsIndices,
                  const std::string &rhs, const List &rhsIndices) override;
  virtual void
  emitTensorProduct(const std::string &result,
                    const std::string &lhs, const std::string &rhs) override;
  virtual void
  emitTensorStack(const std::string &result,
                  const std::list<std::string> &temps) override;
  virtual void
  emitAssignment(const std::string &result, const std::string &expr) override;

  virtual void
  emitAddExpr(const std::string &result,
              const std::string &lhs, const std::string &rhs) override;
  virtual void
  emitSubExpr(const std::string &result,
              const std::string &lhs, const std::string &rhs) override;
  virtual void
  emitMulExpr(const std::string &result,
              const std::string &lhs, const std::string &rhs) override;
  virtual void
  emitDivExpr(const std::string &result,
              const std::string &lhs, const std::string &rhs) override;
};


class TheanoGraphCG : public NumpyGraphCG {
public:
  TheanoGraphCG(const Sema *sema, const std::string &prefix = "T");

  virtual void visitProgramPrologue(const Program *p) override;
  virtual void visitProgramEpilogue(const Program *p) override;

  virtual void visitDeclEpilogue(const Decl *d) override;
};

#endif /* __PYTHON_CODEGEN_H__ */

