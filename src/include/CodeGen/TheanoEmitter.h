
#ifndef __THEANO_EMITTER_H__
#define __THEANO_EMITTER_H__

#include <list>
#include <string>


#include "CodeGen/DirectCodeGen.h"
#include "CodeGen/ExprTree.h"
#include "CodeGen/GraphCodeGen.h"


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
  DECL_VISIT_EXPR_NODE(ScalarMul)
  DECL_VISIT_EXPR_NODE(Div)
  DECL_VISIT_EXPR_NODE(ScalarDiv)
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
  void addExprNode(const Expr *expr, ExprNode *en) {
    CG->addExprNode(expr, en);
  }
  const ExprNode *getExprNode(const Expr *expr) const {
    return CG->getExprNode(expr);
  }

  void addFunctionArgument(const std::string &name) {
    CG->addFunctionArgument(name);
  }
  
  const std::string &getFunctionName() const { return CG->getFunctionName(); }
};

#endif /* __THEANO_EMITTER_H__ */

