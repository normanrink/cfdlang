
#include "CodeGen/IdCopier.h"


void IdCopier::transformAssignments() {
  for (curPos = Assignments.begin(); curPos != Assignments.end(); curPos++) {
    curLhs = static_cast<const IdentifierExpr *>(curPos->lhs);

    // The index structures on the 'lhs' and 'rhs' are definitely
    // incompatible if indices on the 'lhs' are permuted:
    if (curLhs->permute())
      Incompatible = true;
    else
      Incompatible = false;

    Parent = nullptr;
    ChildIndex = -1;

    replacementName = "";
    curPos->rhs->transform(this);
  }
}

void IdCopier::transformChildren(ExprNode *en) {
  ExprNode *savedParent = Parent;
  unsigned savedChildIndex = ChildIndex;

  Parent = en;
  for (int i = 0; i < en->getNumChildren(); i++) {
    ChildIndex = i;
    en->getChild(i)->transform(this);
  }

  ChildIndex = savedChildIndex;
  Parent = savedParent;
}

// handle operations that leave the index structure intact,
// i.e. elementwise and scalar operations:
#define DEF_TRANSFORM_COMPATIBLE_EXPR_NODE(Kind)       \
void IdCopier::transform##Kind##Expr(Kind##Expr *en) { \
  transformChildren(en);                               \
}

DEF_TRANSFORM_COMPATIBLE_EXPR_NODE(Add)
DEF_TRANSFORM_COMPATIBLE_EXPR_NODE(Sub)
DEF_TRANSFORM_COMPATIBLE_EXPR_NODE(Mul)
DEF_TRANSFORM_COMPATIBLE_EXPR_NODE(ScalarMul)
DEF_TRANSFORM_COMPATIBLE_EXPR_NODE(Div)
DEF_TRANSFORM_COMPATIBLE_EXPR_NODE(ScalarDiv)

#undef DECL_TRANSFORM_COMPATIBLE_EXPR_NODE

// handle operations that change the index structure:
#define DEF_TRANSFORM_INCOMPATIBLE_EXPR_NODE(Kind)     \
void IdCopier::transform##Kind##Expr(Kind##Expr *en) { \
  bool savedIncompatible = Incompatible;               \
  Incompatible = true;                                 \
  transformChildren(en);                               \
  Incompatible = savedIncompatible;                    \
}
DEF_TRANSFORM_INCOMPATIBLE_EXPR_NODE(Contraction)
DEF_TRANSFORM_INCOMPATIBLE_EXPR_NODE(Product)
DEF_TRANSFORM_INCOMPATIBLE_EXPR_NODE(Stack)
DEF_TRANSFORM_INCOMPATIBLE_EXPR_NODE(Transposition)

#undef DECL_INCOMPATIBLE_TRANSFORM_EXPR_NODE

void IdCopier::transformIdentifierExpr(IdentifierExpr *en) {
  // the current identifier is different from the 'lhs':
  if (en->getName() != curLhs->getName())
    return;

  // identifiers with the same name must have the same type:
  assert(en->getDims() == curLhs->getDims());

  // index structures of 'lhs' and the current identifier are compatible:
  if (!Incompatible)
    return;

  // if we get here, the current identifier must be replaced

  // if there isn't a replacement identifier yet, create a name for it:
  const std::string temp = (replacementName != "") ? replacementName
                                                   : getTemp();

  ExprNode *newIdNode = getENBuilder()->createIdentifierExpr(temp,
                                                             en->getDims());
  Parent->setChild(ChildIndex, newIdNode);

  // if there wasn't a replacement identifier, assign the 'lhs' to it:
  if (replacementName == "") {
    replacementName = temp;
    ExprNode *copyLhsId =
      getENBuilder()->createIdentifierExpr(curLhs->getName(),
                                           curLhs->getDims());
    ExprNode *newRhsId = getENBuilder()->createIdentifierExpr(replacementName,
                                                              en->getDims());
    // the replacement identifier for the 'rhs' must be assigned the
    // value of the 'lhs' BEFORE the current assignment:
    Assignments.insert(curPos, {newRhsId, copyLhsId});
  }
}
