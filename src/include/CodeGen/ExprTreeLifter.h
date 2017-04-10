
#ifndef __EXPR_TREE_LIFTER_H__
#define __EXPR_TREE_LIFTER_H__

#include <functional>
#include <list>
#include <string>


#include "CodeGen/CodeGen.h"
#include "CodeGen/ExprTree.h"


// This class lifts 'Contraction' and 'Stack' nodes out of expression
// trees and replaces them with temporary variables. This should help
// generate better C code.

class ExprTreeLifter : public ExprTreeTransformer {
public:
  typedef std::function<bool(const ExprNode *)> LiftPredicate;

private:
  CodeGen *CG;

  ExprNode *Parent;
  int ChildIndex;

  const LiftPredicate isNodeToBeLifted;

protected:
  void setParent(ExprNode *p) { Parent = p; }
  ExprNode *getParent() const { return Parent; }

  void setChildIndex(int index) { ChildIndex = index; }
  int getChildIndex() const { return ChildIndex; }

public:
  typedef std::list<CodeGen::Assignment> AssignmentsListTy;

private:
  AssignmentsListTy Assignments;

public:
  ExprTreeLifter(CodeGen *cg, const LiftPredicate &lp)
    : CG(cg), Parent(nullptr), ChildIndex(-1), isNodeToBeLifted(lp) {}
  
  #define DECL_TRANSFORM_EXPR_NODE(Kind)                       \
  virtual void transform##Kind##Expr(Kind##Expr *en) override;

  DECL_TRANSFORM_EXPR_NODE(Add)
  DECL_TRANSFORM_EXPR_NODE(Sub)
  DECL_TRANSFORM_EXPR_NODE(Mul)
  DECL_TRANSFORM_EXPR_NODE(Div)
  DECL_TRANSFORM_EXPR_NODE(Contraction)
  DECL_TRANSFORM_EXPR_NODE(Product)
  DECL_TRANSFORM_EXPR_NODE(Stack)
  DECL_TRANSFORM_EXPR_NODE(Identifier)

  #undef DECL_TRANSFORM_EXPR_NODE

  void transformNode(ExprNode *en);
  void transformChildren(ExprNode *en);
  void liftNode(ExprNode *en);

  const AssignmentsListTy &getAssignments() const {
    return Assignments;
  };
  const AssignmentsListTy::const_iterator assignments_begin() const {
    return Assignments.begin();
  }
  const AssignmentsListTy::const_iterator assignments_end() const {
    return Assignments.end();
  }

  // helper methods that implement functionality from the code generator 'cg':
private:
  std::string getTemp() { return CG->getTemp(); }
  ExprNodeBuilder *getENBuilder() { return CG->getENBuilder(); }
};

#endif /* __EXPR_TREE_LIFTER_H__ */
