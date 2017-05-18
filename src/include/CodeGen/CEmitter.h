
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

  const ExprNode *resultTemp;
  unsigned Indent;

  unsigned IndexCounter;

  const bool RowMajor;

  const bool EmitWrapper;

  const bool FuseElementLoop;

  const bool RestrictPointer;
  const std::string PointerDecl;

private:
  // context for the emission of expression trees:
  std::set<std::string> loopedOverIndices;
  unsigned nestingLevel,
           initialNestingLevel;
  std::vector<std::string> exprIndices, resultIndices;

public:
  CEmitter(CodeGen *cg, bool rowMajor = true, bool emitWrapper = false,
           bool fuseElementLoop = false,
           bool restrictPointer = true,
           const std::string fpTypeName = "double")
  : CG(cg),
    FPTypeName(fpTypeName),
    IndexCounter(0),
    RowMajor(rowMajor),
    EmitWrapper(emitWrapper),
    FuseElementLoop(fuseElementLoop),
    RestrictPointer(restrictPointer),
    PointerDecl(RestrictPointer ? " *restrict " : " *") {}

  void codeGen(const Program *p);
  const std::string &getCode() const { return CG->getCode(); }

protected:
  const std::string &getFPTypeName() const { return FPTypeName; }

  const ExprNode *getResultTemp() const { return resultTemp; }
  void setResultTemp(const ExprNode *temp) { resultTemp = temp; }

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
                          const std::string &temp);

  std::string subscriptString(const std::vector<std::string> &indices,
                              const std::vector<int> &dims) const;
  std::string
  subscriptedIdentifier(const ExprNode *en,
                        const std::vector<std::string> &indices = {}) const;

  void emitLoopHeaderNest(const std::vector<int> &exprDims);
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

#endif /* __C_EMITTER_H__ */

