
#ifndef __ELEMENT_LOOP_FUSER_H__
#define __ELEMENT_LOOP_FUSER_H__

#include <list>
#include <map>
#include <string>


#include "CodeGen/CodeGen.h"
#include "CodeGen/ExprTree.h"


// Fuses the outermost loops - if the outermost loop
// has the same loop bounds (i.e. dimensions) for each
// assignment in the program.

class ElementLoopFuser : public ExprTreeTransformer {
private:
  CodeGen *CG;
  CodeGen::AssignmentsListTy &Assignments;

public:
  ElementLoopFuser(CodeGen *cg)
    : CG(cg),
      Assignments(CG->getAssignments()) {}

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

  void transformChildren(ExprNode *en);

  int transformAssignments();

private:
  bool isDeclaredId(const IdentifierExpr *en) const;
};

#endif /* __ELEMENT_LOOP_FUSER_H__ */
