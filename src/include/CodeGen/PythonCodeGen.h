
#ifndef __PYTHON_CODEGEN_H__
#define __PYTHON_CODEGEN_H__

#include <list>
#include <string>


#include "CodeGen/DirectCodeGen.h"
#include "CodeGen/ExprTree.h"
#include "CodeGen/GraphCodeGen.h"
//#include "CodeGen/PythonFragmentBuilder.h"


class TheanoEmitter : public ExprTreeVisitor {
private:
  CodeGen *CG;

  const std::string ModulePrefix;

  std::string resultTemp;

public:
  TheanoEmitter(CodeGen *cg, const std::string &prefix = "T")
    : CG(cg), ModulePrefix(prefix) {}

  void codeGen(const Program *p);
  const std::string &getCode() const { return CG->getCode(); }

protected:
  const std::string &getModulePrefix() const { return ModulePrefix; }

  std::string getResultTemp() const { return resultTemp; }
  void setResultTemp(const std::string &temp) { resultTemp = temp; }

  #define DECL_VISIT_EXPR_NODE(Kind)                            \
  virtual void visit##Kind##Expr(const Kind##Expr *e) override;

  DECL_VISIT_EXPR_NODE(Add)
  DECL_VISIT_EXPR_NODE(Sub)
  DECL_VISIT_EXPR_NODE(Mul)
  DECL_VISIT_EXPR_NODE(Div)
  DECL_VISIT_EXPR_NODE(Contraction)
  DECL_VISIT_EXPR_NODE(Product)
  DECL_VISIT_EXPR_NODE(Stack)
  DECL_VISIT_EXPR_NODE(Identifier)

  #undef DECL_VISIT_EXPR_NODE

  void visitBinOpExpr(const ExprNode *en, const std::string &op);
  void visitTensordotExpr(const ExprNode *en, const std::string &axes);

// helper methods that implement functionality from the code generator 'cg':
private:
  std::string getTemp() { return CG->getTemp(); }
  void append(const std::string &code) { CG->append(code); }

  const Sema *getSema() const { return CG->getSema(); }
  void addExprNode(const Expr *expr, const ExprNode *en) {
    CG->addExprNode(expr, en);
  }
  const ExprNode *getExprNode(const Expr *expr) const {
    return CG->getExprNode(expr);
  }
};

/*
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
*/
#endif /* __PYTHON_CODEGEN_H__ */

