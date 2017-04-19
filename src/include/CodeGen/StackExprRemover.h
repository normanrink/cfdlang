
#ifndef __STACK_EXPR_REMOVER_H__
#define __STACK_EXPR_REMOVER_H__

#include <list>
#include <map>
#include <string>


#include "CodeGen/CodeGen.h"
#include "CodeGen/ExprTree.h"


// This class removes stack expressions and replaces
// them with indexed identifiers.

class StackExprRemover : public ExprTreeTransformer {
private:
  CodeGen *CG;
  ExprNodeBuilder *ENBuilder;
  CodeGen::AssignmentsListTy &Assignments;

  ExprNode *Parent;
  int ChildIndex;

  CodeGen::AssignmentsListTy::iterator curPos;
  std::map<std::string, const IdentifierExpr *> Replacements;

protected:
  IdentifierExpr *buildReplacement(ExprNode *original);

  void setParent(ExprNode *p) { Parent = p; }
  ExprNode *getParent() const { return Parent; }

  void setChildIndex(int index) { ChildIndex = index; }
  int getChildIndex() const { return ChildIndex; }

public:
  StackExprRemover(CodeGen *cg)
    : CG(cg),
      ENBuilder(CG->getENBuilder()),
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
  DECL_TRANSFORM_EXPR_NODE(Identifier)

  #undef DECL_TRANSFORM_EXPR_NODE

  void transformChildren(ExprNode *en);

  void transformAssignments();

private:
  // helper methods that implement functionality from the code generator 'CG':

  std::string getTemp() { return CG->getTemp(); }
  ExprNodeBuilder *getENBuilder() { return CG->getENBuilder(); }

  bool isDeclaredId(const ExprNode *en) const;
};

#endif /* __STACK_EXPR_REMOVER_H__ */
