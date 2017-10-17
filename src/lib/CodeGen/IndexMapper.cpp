
#include "CodeGen/IndexMapper.h"


void IndexMapper::visitProductExpr(const ProductExpr *en) {
  const IndexSetTy savedExprIndices = exprIndices;

  const ExprNode *lhsExpr = en->getChild(0);
  const ExprNode *rhsExpr = en->getChild(1);

  const std::vector<int> &lhsDims = lhsExpr->getDims();
  const std::vector<int> &rhsDims = rhsExpr->getDims();

  IndexSetTy lhsIndices, rhsIndices;

  for (int i = 0; i < lhsDims.size(); i++) {
    lhsIndices.push_back(exprIndices[i]);
  }
  for (int i = 0; i < rhsDims.size(); i++) {
    rhsIndices.push_back(exprIndices[lhsDims.size() + i]);
  }

  exprIndices = lhsIndices;
  lhsExpr->visit(this);

  exprIndices = rhsIndices;
  rhsExpr->visit(this);

  exprIndices = savedExprIndices;
}

void IndexMapper::visitStackExpr(const StackExpr *en) {
  const IndexSetTy savedExprIndices = exprIndices;

  IndexSetTy childExprIndices;
  // skip the first index:
  for (int i = 1/* The '1' is intended! */; i < exprIndices.size(); i++) {
    childExprIndices.push_back(exprIndices[i]);
  }

  exprIndices = childExprIndices;
  visitChildren(en);

  exprIndices = savedExprIndices;
}

void IndexMapper::visitTranspositionExpr(const TranspositionExpr *en) {
  const IndexSetTy savedExprIndices = exprIndices;

  IndexSetTy transposedExprIndices = exprIndices;
  const std::vector<std::vector<int>> &indexPairs = en->getIndexPairs();
  // traverse index pairs in reverse order and apply transpositions
  // (previously, e.g. in 'Sema', index pairs are traversed in order to
  // synthesize the type of an expression bottom-up; now going top-down
  // to construct the 'exprIndices')
  for (auto pi = indexPairs.rbegin(); pi != indexPairs.rend(); pi++) {
    const auto p = *pi;
    assert(p.size() == 2);
    const std::string index0 = transposedExprIndices[p[0]];
    transposedExprIndices[p[0]] = transposedExprIndices[p[1]];
    transposedExprIndices[p[1]] = index0;
  }

  exprIndices = transposedExprIndices;
  visitChildren(en);

  exprIndices = savedExprIndices;
}

void IndexMapper::visitContractionExpr(const ContractionExpr *en) {
  const IndexSetTy savedExprIndices = exprIndices;

  const ExprNode *lhsExpr = en->getChild(0);
  const ExprNode *rhsExpr = en->getChild(1);

  const std::vector<int> &lhsDims = lhsExpr->getDims();
  const std::vector<int> &rhsDims = rhsExpr->getDims();

  IndexSetTy lhsIndices, rhsIndices;
  int exprIdxCounter = 0;
  // determine which indices belong to the 'lhs':
  {
    int contrIdxCounter = 0;

    for (int i = 0; i < lhsDims.size(); i++) {
      std::string index;

      if (i == en->getLeftIndices()[contrIdxCounter]) {
        index = Dummy;
        contrIdxCounter += 1;
      } else {
        index = exprIndices[exprIdxCounter++];
      }

      lhsIndices.push_back(index);
    }
  }
  // determine which indices belong to the 'rhs':
  {
    int contrIdxCounter = 0;

    for (int i = 0; i < rhsDims.size(); i++) {
      std::string index;

      if (i == en->getRightIndices()[contrIdxCounter]) {
        index = Dummy;
        contrIdxCounter += 1;
      } else {
        index = exprIndices[exprIdxCounter++];
      }

      rhsIndices.push_back(index);
    }
  }

  exprIndices = lhsIndices;
  lhsExpr->visit(this);

  exprIndices = rhsIndices;
  rhsExpr->visit(this);

  exprIndices = savedExprIndices;
}

IndexMapper::IndexSetTy IndexMapper::getLhsIndices(const ExprNode *en) const {
  assert(en->isIdentifier());

  IndexSetTy allIndices;
  for (unsigned i = 0; i < en->getNumIndices(); i++)
    allIndices.push_back(en->getIndex(i));
  for (unsigned i = 0; i < exprIndices.size(); i++)
    allIndices.push_back(exprIndices.at(i));

  const IdentifierExpr *id = static_cast<const IdentifierExpr *>(en);
  if (id->permute()) {
    // the indices must be permuted going through the transpositions forwards:
    // (when 'TranspositionExpr' is emitted (on the 'rhs' of assignments), the
    // index pairs are traversed backwards; here the permutations appear on the
    // 'lhs' of assignments, hence index pairs are traversed forwards)
    const auto &indexPairs = id->getIndexPairs();
    for (auto pi = indexPairs.begin(); pi != indexPairs.end(); pi++) {
      const auto p = *pi;
      assert(p.size() == 2);

      const std::string index0 = allIndices[p[0]];
      allIndices[p[0]] = allIndices[p[1]];
      allIndices[p[1]] = index0;
    }
  }

  return allIndices;
}

bool IndexMapper::areIndicesAdjacent(const std::string &i0,
                                     const std::string &i1,
                                     const IndexSetTy &indices) {
  bool i0contained = false, i1contained = false;

  for (int i = 0; i < indices.size(); i++) {
    if (i0 == indices[i]) i0contained = true;
    if (i1 == indices[i]) i1contained = true;
  }
  if (!i0contained && !i1contained)
    return true;

  for (int i = 0; i < (indices.size()-1); i++) {
    if(i0 == indices[i] && i1 == indices[i+1])
      return true;
  }

  return false;
}

bool IndexMapper::areIndicesAlwaysAdjacent(const std::string &i0,
                                           const std::string &i1) const {
  for (auto i = getIdToIdxSetMap().begin(), e = getIdToIdxSetMap().end();
       i != e; i++) {
    const IdentifierExpr *id = i->first;
    const IndexSetTy &indices = i->second;

    if(!areIndicesAdjacent(i0, i1, indices))
      return false;
  }

  return true;
}
