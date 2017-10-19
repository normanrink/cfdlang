
#ifndef __EXPR_TREE_LIFTER_H__
#define __EXPR_TREE_LIFTER_H__

#include <functional>
#include <list>
#include <string>


#include "CodeGen/CodeGen.h"
#include "CodeGen/ExprTree.h"


// This class can lift nodes out of expression trees and replace them with
// temporary variables. This can help generate better C code.

class ExprTreeLifter : public ExprTreeTransformer {
public:
  typedef std::function<bool(const ExprNode *, const ExprNode *)> LiftPredicate;

private:
  CodeGen *CG;
  CodeGen::AssignmentsListTy &Assignments;

  ExprNode *Root;
  ExprNode *Parent;
  int ChildIndex;

  CodeGen::AssignmentsListTy::iterator curPos;

  const LiftPredicate isNodeToBeLifted;

protected:
  void setRoot(ExprNode *r) { Root = r; }
  ExprNode *getRoot() const { return Root; }

  void setParent(ExprNode *p) { Parent = p; }
  ExprNode *getParent() const { return Parent; }

  void setChildIndex(int index) { ChildIndex = index; }
  int getChildIndex() const { return ChildIndex; }

public:
  ExprTreeLifter(CodeGen *cg, const LiftPredicate &lp)
    : CG(cg),
      Assignments(CG->getAssignments()),
      isNodeToBeLifted(lp) {}
  
  #define DECL_TRANSFORM_EXPR_NODE(Kind)                       \
  virtual void transform##Kind##Expr(Kind##Expr *en) override;

  DECL_TRANSFORM_EXPR_NODE(Add)
  DECL_TRANSFORM_EXPR_NODE(Sub)
  DECL_TRANSFORM_EXPR_NODE(Mul)
  DECL_TRANSFORM_EXPR_NODE(ScalarMul)
  DECL_TRANSFORM_EXPR_NODE(Div)
  DECL_TRANSFORM_EXPR_NODE(ScalarDiv)
  DECL_TRANSFORM_EXPR_NODE(Contraction)
  DECL_TRANSFORM_EXPR_NODE(Product)
  DECL_TRANSFORM_EXPR_NODE(Stack)
  DECL_TRANSFORM_EXPR_NODE(Transposition)
  DECL_TRANSFORM_EXPR_NODE(Identifier)

  #undef DECL_TRANSFORM_EXPR_NODE

  void transformNode(ExprNode *en);
  void transformChildren(ExprNode *en);
  void liftNode(ExprNode *en);

  void transformAssignments();

  // helper methods that implement functionality from the code generator 'cg':
private:
  std::string getTempWithDims(const std::vector<int> &dims) {
    return CG->getTempWithDims(dims);
  }
  void freeTempWithDims(const std::string &name,
                        const std::vector<int> &dims) {
    CG->freeTempWithDims(name, dims);
  }
  
  ExprNodeBuilder *getENBuilder() { return CG->getENBuilder(); }
};

#endif /* __EXPR_TREE_LIFTER_H__ */
