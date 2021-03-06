
#ifndef __C_EMITTER_H__
#define __C_EMITTER_H__

#include <list>
#include <vector>
#include <string>


#include "CodeGen/CodeGen.h"
#include "CodeGen/ExprTree.h"
#include "Sema/TensorType.h"


namespace CFDlang {

class CEmitter : public ExprTreeVisitor {
private:
  CodeGen *CG;

  const std::string FPTypeName;

  const ExprNode *resultTemp;
  unsigned Indent;

  unsigned IndexCounter;

  const bool RowMajor;

  const bool EmitWrapper;

  const bool RestrictPointer;

  const bool IccPragmas;

  const bool OMPPragmas;

private:
  // context for the emission of expression trees:
  std::set<std::string> loopedOverIndices;
  unsigned nestingLevel,
           initialNestingLevel;
  std::vector<std::string> exprIndices, resultIndices;

  std::string ElementIndex;

public:
  CEmitter(CodeGen *cg, bool rowMajor = true, bool emitWrapper = false,
           bool restrictPointer = true,
           bool iccPragmas = false,
           bool ompPragmas = false,
           const std::string fpTypeName = "double")
  : CG(cg),
    FPTypeName(fpTypeName),
    IndexCounter(0),
    RowMajor(rowMajor),
    EmitWrapper(emitWrapper),
    RestrictPointer(restrictPointer),
    IccPragmas(iccPragmas),
    OMPPragmas(ompPragmas) {}

  void codeGen(const Program *p);
  const std::string &getCode() const { return CG->getCode(); }

protected:
  const std::string &getFPTypeName() const { return FPTypeName; }

  const ExprNode *getResultTemp() const { return resultTemp; }
  void setResultTemp(const ExprNode *temp) { resultTemp = temp; }

  unsigned getIndent() const { return Indent; }
  void setIndent(unsigned indent) { Indent = indent; }

  std::string getIndex();

  void emitSignature(std::vector<std::vector<int>> &argumentsDims);

  void emitForLoopHeader(unsigned indent,
                         const std::string &indexVar, const std::string &bound);
  void emitForLoopHeader(unsigned indent,
                         const std::string &indexVar, int intBound,
                         bool unroll = false, bool simd = false);
  void emitForLoopFooter(unsigned indent);

  void emitTempDefinition(unsigned indent,
                          const std::string &temp);

  std::string subscriptString(const std::vector<std::string> &indices,
                              const std::vector<int> &dims) const;
  std::string dimsString(const std::vector<int> &dims,
                         bool emitRestrict = false) const;

  void updateWithElemInfo(std::vector<std::string> &indices,
                          std::vector<int> &dims,
                          const std::string &name) const;
  std::string
  subscriptedIdentifier(const ExprNode *en,
                        const std::vector<std::string> &indices = {}) const;

  void emitLoopHeaderNest(const std::vector<int> &exprDims,
                          bool unroll = false, bool simd = false);
  void emitLoopFooterNest();

  std::string visitChildExpr(const ExprNode *en);

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
  DECL_VISIT_EXPR_NODE(Transposition)
  DECL_VISIT_EXPR_NODE(Identifier)

  #undef DECL_VISIT_EXPR_NODE

  void visitBinOpExpr(const ExprNode *en, const std::string &op);
  void visitTopLevelIdentifier(const ExprNode *en);

  // helper methods:
private:
  std::string getTemp() { return CG->getTemp(); }
  std::string getTempWithDims(const std::vector<int> &dims) {
    return CG->getTempWithDims(dims);
  }
  void freeTempWithDims(const std::string &name,
                        const std::vector<int> &dims) {
    CG->freeTempWithDims(name, dims);
  }

  void append(const std::string &code) { CG->append(code); }

  const Sema *getSema() const { return CG->getSema(); }

  const std::vector<int> &getDims(const ExprNode *en) const {
    return en->getDims();
  }

  void addFunctionArgument(const std::string &name) {
    CG->addFunctionArgument(name);
  }
  int getNumFunctionArguments() const {
    return CG->getFunctionArguments().size();
  }

  const std::string &getFunctionName() const { return CG->getFunctionName(); }
  std::string getFunctionNameWrapped() const {
    std::string name = CG->getFunctionName();
    if (EmitWrapper)
      name += "_wrapped";

    return name;
  }
};

};

#endif /* __C_EMITTER_H__ */
