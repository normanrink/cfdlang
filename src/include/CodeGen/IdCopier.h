
#ifndef __ID_COPIER_H__
#define __ID_COPIER_H__

#include <functional>
#include <list>
#include <string>


#include "CodeGen/CodeGen.h"
#include "CodeGen/ExprTree.h"


namespace CFDlang {

// This class lifts identifiers out of expression trees if they appear on
// both the 'lhs' and the 'rhs' in conflicting ways (i.e. with different
// index structures).
//
// NOTE that the 'ExprLifter' cannot simply be used for the same purpose
// since the 'IdCopier' must retain more state about the current expression
// tree and identifier.

class IdCopier : public ExprTreeTransformer {
private:
  CodeGen *CG;
  CodeGen::AssignmentsListTy &Assignments;

  const IdentifierExpr *curLhs;
  ExprNode *Parent;
  int ChildIndex;
  bool Incompatible;
  std::string replacementName;

  CodeGen::AssignmentsListTy::iterator curPos;

public:
  IdCopier(CodeGen *cg)
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

  void transformNode(ExprNode *en);
  void transformChildren(ExprNode *en);
  void liftNode(ExprNode *en);

  void transformAssignments();

  // helper methods that implement functionality from the code generator 'cg':
private:
  std::string getTemp() { return CG->getTemp(); }
  ExprNodeBuilder *getENBuilder() { return CG->getENBuilder(); }
};

};

#endif /* __ID_COPIER_H__ */
