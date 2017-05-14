
#include <vector>
#include <list>


#include "CodeGen/ElementLoopFuser.h"


#define INVALID_DIMENSION -1


bool ElementLoopFuser::isDeclaredId(const IdentifierExpr *en) const {
  for (const auto *id : CG->getDeclaredIds()) {
    if (en->getName() == id->getName())
      return true;
  }

  return false;
}

int ElementLoopFuser::transformAssignments() {
  // first, see if outer loops have a common dimension:
  bool haveCommonDim;
  int commonDim = INVALID_DIMENSION;

  for (const auto &a: Assignments) {
    const IdentifierExpr *lhs = static_cast<const IdentifierExpr *> (a.lhs);
    const std::vector<int> &lhsDims = lhs->getDims();
    // since stack expressions may have been removed,
    // there can be fixed indices:
    const int numFixedIndices = lhs->getNumIndices();
    int outerDimIndex;
    if (lhs->permute()) {
      // fixed indices may be moved by permutations
      // (i.e. by index transpositions):
      std::list<int> transposedFixedIndices;
      for (int i = 0; i < numFixedIndices; i++) {
        int res = i;
        const auto &indexPairs = lhs->getIndexPairs();
        for (auto pi = indexPairs.begin(); pi != indexPairs.end(); pi++) {
          const auto p = *pi;
          assert(p.size() == 2);

          if (p[0] == res) res = p[1];
          else if (p[1] == res) res = p[0];
        }
        transposedFixedIndices.push_back(res);
      }
      transposedFixedIndices.sort();

      std::vector<int> fixedIndices;
      for (const auto &ind: transposedFixedIndices)
        fixedIndices.push_back(ind);

      outerDimIndex = 0;
      for (int i = 0; i < fixedIndices.size(); i++) {
        if (i != fixedIndices[i])
          break;

        outerDimIndex = i+1;
      }
    } else {
      outerDimIndex = numFixedIndices;
    }

    const int outerDim = lhsDims[outerDimIndex];

    if (commonDim == INVALID_DIMENSION) {
      haveCommonDim = true;
      commonDim = outerDim;
    } else if (commonDim != outerDim) {
      haveCommonDim = false;
      break;
    }
  }

  if (!haveCommonDim)
    return 0;

  // second, mark the positions of the common loop index in 'IdentiferExpr';
  // only modify variables that have not been explicitly declared:
  for (auto &a: Assignments) {
    IdentifierExpr *lhs = static_cast<IdentifierExpr *>(a.lhs);
    ExprNode *rhs = a.rhs;

    if (!isDeclaredId(lhs))
      lhs->setElementIndexPosition(lhs->getNumIndices());

    rhs->transform(this);
  }

  return commonDim;
}

void ElementLoopFuser::transformChildren(ExprNode *en) {
  for (int i = 0; i < en->getNumChildren(); i++)
    en->getChild(i)->transform(this);
}

#define DEF_TRANSFORM_EXPR_NODE(Kind)                          \
void ElementLoopFuser::transform##Kind##Expr(Kind##Expr *en) { \
  transformChildren(en);                                       \
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

void ElementLoopFuser::transformIdentifierExpr(IdentifierExpr *en) {
  // only modify variables that have not been explicitly declared:
  if (!isDeclaredId(en))
    en->setElementIndexPosition(en->getNumIndices());
}
