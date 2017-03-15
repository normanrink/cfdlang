
#ifndef __PYTHON_CODEGEN_H__
#define __PYTHON_CODEGEN_H__

#include <map>
#include <string>


#include "CodeGen/DirectCodeGen.h"
#include "CodeGen/GraphCodeGen.h"
#include "Sema/TensorType.h"


class NumpyDirectCodeGen : public DirectCodeGen {
private:
  const std::string ModulePrefix;

protected:
  const std::string &getModulePrefix() const { return ModulePrefix; }

public:
  NumpyDirectCodeGen(const Sema *sema, const std::string &prefix = "np");

  virtual void visitProgramPrologue(const Program *p) override;

  virtual void visitDeclEpilogue(const Decl *d) override;
  virtual void visitStmtEpilogue(const Stmt *s) override;

  virtual const std::string
  visitContractionEpilogue(const Expr *e,
                           const std::string &lhs, const std::string &rhs,
                           const TupleList &LeftAndRightInds);

  virtual void visitTensorExprEpilogue(const BinaryExpr *be) override;
  virtual void visitIntegerEpilogue(const Integer *i) override;
  virtual void visitBrackExprEpilogue(const BrackExpr *be) override;

  const std::string getTensorDotString(const std::string &r,
                                       const std::string &t0,
                                       const std::string &t1,
                                       const std::string axes = "") const;
};


class TheanoDirectCodeGen : public NumpyDirectCodeGen {
private:
  std::map<const TensorType *, std::string> TypeTemps;

  void constructTypes();

public:
  TheanoDirectCodeGen(const Sema *sema, const std::string &prefix = "T");

  virtual void visitProgramPrologue(const Program *p) override;
  virtual void visitProgramEpilogue(const Program *p) override;

  virtual void visitDeclEpilogue(const Decl *d) override;
};


class NumpyGraphCG : public GraphCodeGen {
private:
  const std::string ModulePrefix;

protected:
  const std::string &getModulePrefix() const { return ModulePrefix; }

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
};


class TheanoGraphCG : public NumpyGraphCG {
private:
  std::map<const TensorType *, std::string> TypeTemps;

  void constructTypes();

public:
  TheanoGraphCG(const Sema *sema, const std::string &prefix = "T");

  virtual void visitProgramPrologue(const Program *p) override;
  virtual void visitProgramEpilogue(const Program *p) override;

  virtual void visitDeclEpilogue(const Decl *d) override;
};

#endif /* __PYTHON_CODEGEN_H__ */

