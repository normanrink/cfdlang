
#include <list>
#include <map>
#include <vector>


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
  std::map<const CodeGen::Assignment *, int> commonDimIndices;

  for (auto &a: Assignments) {
    IdentifierExpr *lhs = static_cast<IdentifierExpr *>(a.lhs);
    std::vector<int> lhsDims = lhs->getDims();
    // since stack expressions may have been removed,
    // there can be fixed indices:
    const int numFixedIndices = lhs->getNumIndices();

    // since transpositions may have been moved over to the 'lhs',
    // the dimensions of the 'lhs' may have to be permuted accordingly:
    if (lhs->permute()) {
      // permute dimensions:
      const auto &indexPairs = lhs->getIndexPairs();
      for (auto pi = indexPairs.rbegin(); pi != indexPairs.rend(); pi++) {
        const auto p = *pi;
        assert(p.size() == 2);

        const int dim0 = lhsDims[p[0]];
        lhsDims[p[0]] = lhsDims[p[1]];
        lhsDims[p[1]] = dim0;
      }
    }

    // after potentially permuting the dimensions of the 'lhs',
    // the "element index" is either
    // (a) the first index that has not been fixed during stack removal
    //     (if the layout is row-major)
    // (b) the last index (if the layout is column-major)
    commonDimIndices[&a] = RowMajor ? numFixedIndices : (lhsDims.size()-1);

    const int outerDim = lhsDims[commonDimIndices[&a]];

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

  // second, mark the index positions of the common loop, only
  // modify variables that have not been explicitly declared:
  for (auto &a: Assignments) {
    CommonDimIndex = commonDimIndices[&a];

    IdentifierExpr *lhs = static_cast<IdentifierExpr *>(a.lhs);
    if (!isDeclaredId(lhs))
      lhs->setElementIndexPosition(CommonDimIndex);

    a.rhs->transform(this);
  }

  return commonDim;
}

void ElementLoopFuser::transformExprWithRepeatedDims(ExprNode *en) {
  for (int i = 0; i < en->getNumChildren(); i++)
    en->getChild(i)->transform(this);
}

void ElementLoopFuser::transformExprWithSequentialDims(ExprNode *en) {
  if (RowMajor) {
    const int lhsRank = en->getChild(0)->getDims().size();

    if (CommonDimIndex < lhsRank) {
      en->getChild(0)->transform(this);
    } else {
      CommonDimIndex -= lhsRank;
      en->getChild(1)->transform(this);
      CommonDimIndex += lhsRank;
    }
  } else {
    const int rhsRank = en->getChild(1)->getDims().size();

    if (CommonDimIndex >= rhsRank) {
      CommonDimIndex -= rhsRank;
      en->getChild(0)->transform(this);
      CommonDimIndex += rhsRank;
    } else {
      en->getChild(1)->transform(this);
    }
  }
}


#define DEF_TRANSFORM_EXPR_NODE(Kind)                          \
void ElementLoopFuser::transform##Kind##Expr(Kind##Expr *en) { \
  transformExprWithRepeatedDims(en);                           \
}

DEF_TRANSFORM_EXPR_NODE(Add)
DEF_TRANSFORM_EXPR_NODE(Sub)
DEF_TRANSFORM_EXPR_NODE(Mul)
DEF_TRANSFORM_EXPR_NODE(Div)

#undef DEF_TRANSFORM_EXPR_NODE

void ElementLoopFuser::transformScalarMulExpr(ScalarMulExpr *en) {
  en->getChild(1)->transform(this);
}

void ElementLoopFuser::transformScalarDivExpr(ScalarDivExpr *en) {
  en->getChild(0)->transform(this);
}

#define DEF_TRANSFORM_EXPR_NODE(Kind)                          \
void ElementLoopFuser::transform##Kind##Expr(Kind##Expr *en) { \
  transformExprWithSequentialDims(en);                         \
}

DEF_TRANSFORM_EXPR_NODE(Contraction)
DEF_TRANSFORM_EXPR_NODE(Product)

#undef DEF_TRANSFORM_EXPR_NODE

void ElementLoopFuser::transformStackExpr(StackExpr *en) {
  if(CommonDimIndex == 0) {
    assert(0 && "iternal error: should not be here");
    return;
  }

  CommonDimIndex -= 1;
  for (int i = 0; i < en->getNumChildren(); i++)
    en->getChild(i)->transform(this);
  CommonDimIndex += 1;
}

void ElementLoopFuser::transformTranspositionExpr(TranspositionExpr *en) {
  const int savedCommonDimIndex = CommonDimIndex;

  const auto &indexPairs = en->getIndexPairs();
  for (auto pi = indexPairs.begin(); pi != indexPairs.end(); pi++) {
    const auto p = *pi;
    assert(p.size() == 2);

    if (p[0] == CommonDimIndex) CommonDimIndex = p[1];
    else if (p[1] == CommonDimIndex) CommonDimIndex = p[0];
  }

  en->getChild(0)->transform(this);

  CommonDimIndex = savedCommonDimIndex;
}

void ElementLoopFuser::transformIdentifierExpr(IdentifierExpr *en) {
  // only modify variables that have not been explicitly declared:
  if (!isDeclaredId(en))
    en->setElementIndexPosition(CommonDimIndex);
}
