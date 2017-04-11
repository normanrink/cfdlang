
#ifndef __C_EMITTER_H__
#define __C_EMITTER_H__

#include <list>
#include <vector>
#include <string>


#include "CodeGen/CodeGen.h"
#include "CodeGen/ExprTree.h"
#include "Sema/TensorType.h"


class CEmitter : public ExprTreeVisitor {
private:
  CodeGen *CG;

  const std::string FPTypeName;

  std::string resultTemp;
  unsigned Indent;

  unsigned IndexCounter;

  const bool RowMajor;

private:
  // context for the emission of expression trees:
  std::set<std::string> loopedOverIndices;
  unsigned nestingLevel,
           initialNestingLevel;
  std::vector<std::string> exprIndices;

public:
  CEmitter(CodeGen *cg, bool rowMajor = true,
           const std::string fpTypeName = "double")
  : CG(cg),
    FPTypeName(fpTypeName),
    IndexCounter(0),
    RowMajor(rowMajor) {}

  void codeGen(const Program *p);
  const std::string &getCode() const { return CG->getCode(); }

protected:
  const std::string &getFPTypeName() const { return FPTypeName; }

  std::string getResultTemp() const { return resultTemp; }
  void setResultTemp(const std::string &temp) { resultTemp = temp; }

  unsigned getIndent() const { return Indent; }
  void setIndent(unsigned indent) { Indent = indent; }

  std::string getIndex();

  void emitSignature();

  void emitForLoopHeader(unsigned indent,
                         const std::string &indexVar, const std::string &bound);
  void emitForLoopHeader(unsigned indent,
                         const std::string &indexVar, int intBound);
  void emitForLoopFooter(unsigned indent);

  void emitTempDefinition(unsigned indent,
                          const std::string &temp,
                          const std::string &init = "");

  std::string subscriptString(const std::vector<std::string> &indices,
                              const std::vector<int> &dims) const;

  void emitLoopHeaderNest(const std::vector<int> &exprDims);
  void emitLoopFooterNest();

  std::string visitChildExpr(const ExprNode *en,
                             const std::vector<int> &childExprDims);

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

  // helper methods:
private:
  std::string getTemp() { return CG->getTemp(); }
  void append(const std::string &code) { CG->append(code); }

  const Sema *getSema() const { return CG->getSema(); }

  ExprNode *getExprNode(const Expr *expr) const {
    return CG->getExprNode(expr);
  }

  const std::vector<int> &getDims(const ExprNode *en) const {
    return en->getDims();
  }
};

#endif /* __C_EMITTER_H__ */

