
#include "CodeGen/ExprTreeLifter.h"


void ExprTreeLifter::transformAssignments() {
  for (curPos = Assignments.begin(); curPos != Assignments.end(); curPos++) {
    ExprNode *rhs = curPos->rhs;

    setRoot(rhs);
    setParent(nullptr);
    setChildIndex(-1);
    rhs->transform(this);
  }
}

void ExprTreeLifter::transformNode(ExprNode *en) {
  if (isNodeToBeLifted(en, getRoot()))
    liftNode(en);
  else
    transformChildren(en);
}

void ExprTreeLifter::liftNode(ExprNode *en) {
  ExprNode *root = getRoot();
  ExprNode *parent = getParent();
  unsigned childIndex = getChildIndex();

  if (parent == nullptr) {
    // node 'en' is at the top of the tree
    // --> nothing to do here but visit children
    setParent(en);
    transformChildren(en);
    setParent(nullptr);
  } else {
    const std::string temp = getTemp();
    ExprNode *newNode = getENBuilder()->createIdentifierExpr(temp,
                                                             en->getDims());
    // replace sub-expression 'en' (which is to be lifted)
    // with the identifier 'temp': 
    parent->setChild(childIndex, newNode);
    // recursively lift expressions out of the sub-expression 'en':
    setRoot(en);
    setParent(nullptr);
    setChildIndex(-1);
    transformChildren(en);

    // add a new assignment that assigns the lifted sub-expression ('en')
    // to the new identifier 'temp':
    ExprNode *newLHS = getENBuilder()->createIdentifierExpr(temp,
                                                            en->getDims());
    // NOTE that the new assignment is inserted before 'curPos' on purpose:
    Assignments.insert(curPos, {newLHS, en});

    // now that 'en' has been lifted, the expression tree
    // rooted at 'root' must be re-visited:
    setRoot(root);
    setParent(nullptr);
    setChildIndex(-1);
    transformChildren(root);
  }

  setChildIndex(childIndex);
  setParent(parent);
  setRoot(root);
}

void ExprTreeLifter::transformChildren(ExprNode *en) {
  ExprNode *parent = getParent();
  unsigned childIndex = getChildIndex();

  setParent(en);
  for (int i = 0; i < en->getNumChildren(); i++) {
    setChildIndex(i);
    en->getChild(i)->transform(this);
  }

  setChildIndex(childIndex);
  setParent(parent);
}

#define DEF_TRANSFORM_EXPR_NODE(Kind)                        \
void ExprTreeLifter::transform##Kind##Expr(Kind##Expr *en) { \
  transformNode(en);                                         \
}

DEF_TRANSFORM_EXPR_NODE(Add)
DEF_TRANSFORM_EXPR_NODE(Sub)
DEF_TRANSFORM_EXPR_NODE(Mul)
DEF_TRANSFORM_EXPR_NODE(ScalarMul)
DEF_TRANSFORM_EXPR_NODE(Div)
DEF_TRANSFORM_EXPR_NODE(ScalarDiv)
DEF_TRANSFORM_EXPR_NODE(Contraction)
DEF_TRANSFORM_EXPR_NODE(Product)
DEF_TRANSFORM_EXPR_NODE(Stack)
DEF_TRANSFORM_EXPR_NODE(Transposition)

#undef DECL_TRANSFORM_EXPR_NODE

void ExprTreeLifter::transformIdentifierExpr(IdentifierExpr *en) {
  // nothing to be done
  return;
}
