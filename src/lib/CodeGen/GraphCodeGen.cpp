
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>


#include "AST/AST.h"
#include "CodeGen/GraphCodeGen.h"
#include "Sema/Sema.h"
#include "Sema/TensorType.h"


GraphCodeGen::GraphCodeGen(const Sema *sema)
  : CodeGen(sema) {}

GraphCodeGen::~GraphCodeGen() {
  for (const auto &it : Graphs) delete it.second;
}

void GraphCodeGen::visitProgram(const Program *p) {
  ASTVisitor::visitProgram(p);
  
  // TODO: replace this with meaningful code generation:
  for(const auto &g : Graphs) {
    const std::string tmpFileName = std::tmpnam(nullptr) + std::string(".dot");
    std::ofstream of(tmpFileName);
    std::cout << "Writing graph to file \'" << tmpFileName << "\' ... \n";
    g.second->plot(of);
    of.close();
  }
}

void GraphCodeGen::visitStmt(const Stmt *s) {
  const Expr *expr = s->getExpr();

  curGraph = new GCG_Graph;
  curLegs.clear();
  expr->visit(this);

  const std::string &name = s->getIdentifier()->getName();
  Graphs.push_back(GCG_LabeledGraph(name, curGraph));
}

void GraphCodeGen::visitIdentifier(const Identifier *id) {
  std::stringstream ss;
  ss << std::hex << id;

  const TensorType *type = getSema()->getType(id);
  const int rank = type->getRank();
  GCG_Node *n = curGraph->getNode(StringID(ss.str(), id->getName()),
                                  rank);

  for (int i = 0; i < rank; i++)
    curLegs.push_back(GCG_Edge::NodeIndexPair(n, i));
}

void GraphCodeGen::visitInteger(const Integer *i) {
  assert(0 &&
         "internal error: integer should not be visited for graph generation");
}

void GraphCodeGen::visitParenExpr(const ParenExpr *pe) {
  const Expr *e = pe->getExpr();
  e->visit(this);
}

void GraphCodeGen::visitBrackExpr(const BrackExpr *be) {
  const ExprList &exprs = *be->getExprs();
  std::stringstream ssID, ssLabel;
  ssID << std::hex << be;

  GCG_Graph *graph = curGraph;
  GCG_Legs legs = curLegs;

  ssLabel << "[";
  for (unsigned i = 0; i < exprs.size(); i++) {
    curGraph = new GCG_Graph;
    curLegs.clear();
    exprs[i]->visit(this);

    std::string t = getTemp();
    Graphs.push_back(GCG_LabeledGraph(t, curGraph));
    
    if (i > 0) ssLabel << ", ";
    ssLabel << t;
  }
  ssLabel << "]";

  curGraph = graph;
  const TensorType *type = getSema()->getType(be);
  const int rank = type->getRank();
  GCG_Node *n = curGraph->getNode(StringID(ssID.str(), ssLabel.str()),
                                  rank);

  curLegs = legs;
  for (int i = 0; i < rank; i++)
    curLegs.push_back(GCG_Edge::NodeIndexPair(n, i));
} 

void GraphCodeGen::visitBinaryExpr(const BinaryExpr *be) {
  switch (be->getNodeType()) {
  case NT_TensorExpr: {
    const Expr *left = be->getLeft();
    left->visit(this);
    const Expr *right = be->getRight();
    right->visit(this);
    return;
  }
  case NT_DotExpr: {
    const BinaryExpr *tensor = extractTensorExprOrNull(be->getLeft());
    if (!tensor)
      assert(0 && "internal error: cannot handle general contractions yet");

    TupleList contractionsList;
    if (!getSema()->isListOfLists(be->getRight(), contractionsList))
      assert(0 && "internal error: cannot have a non-list here");

    if (contractionsList.empty())
      assert(0 && "internal error: cannot have an empty list here");

    visitContraction(tensor, contractionsList);
    return;
  }
  default:
    assert(0 && "internal error: invalid binary expression");
  }
}

void GraphCodeGen::visitContraction(const Expr *e, const TupleList &indices) {
  if (indices.empty()) {
    e->visit(this);
    return;
  }
  
  const BinaryExpr *tensor = extractTensorExprOrNull(e);
  if (!tensor)
    assert(0 && "internal error: cannot handle general contractions yet");

  if (!isPairList(indices))
    assert(0 && "internal error: only pairs of indices can be contracted");

  const Expr *tensorL = tensor->getLeft();
  const Expr *tensorR = tensor->getRight();
  const TensorType *typeL = getSema()->getType(tensorL);
  int rankL = typeL->getRank();

  TupleList contrL, contrR, contrMixed;
  // classify index pairs into the following categories:
  // - belonging to contractions of the left sub-expression ('contrL')
  // - belonging to contractions of the right sub-expression ('contrR')
  // - having one index from each sub-expression ('contrMixed')
  partitionPairList(rankL, indices, contrL, contrR, contrMixed);

  GCG_Legs savedLegs = curLegs;
  curLegs.clear();
  visitContraction(tensorL, contrL);
  GCG_Legs LeftLegs = curLegs;

  // determine the rank of the resulting left sub-expression after
  // contraction has been performed over the set of index pairs 'contrL':
  int rankContractedL = rankL - 2*contrL.size();

  // the index pairs of the right sub-expression must be adjusted by
  // the rank of the left sub-expression:
  TupleList shiftedR = contrR;
  shiftTupleList(-rankContractedL, shiftedR);

  curLegs.clear();
  visitContraction(tensorR, shiftedR);
  GCG_Legs RightLegs = curLegs;

  if (contrMixed.empty())
    return;

  List indL, indR;
  unpackPairList(contrMixed, indL, indR);
  // only contractions in 'contrL' affect the adjustments
  // of the left indices in 'indL':
  adjustForContractions(indL, contrL);
  // adjustments of the right indices in 'indR' are affected by
  // the contractions in both 'contrL' and 'contrR':
  adjustForContractions(indR, contrL); adjustForContractions(indR, contrR);
  // the indices to be contracted over in the right sub-expression
  // must be relative to the first index of the right sub-expresion:
  shiftList(-rankContractedL, indR);
  
  assert(indL.size() == indR.size() &&
         "internal error: mis-matched numbers of indices to be contracted");

  for (int k = 0; k < indL.size(); k++) {
    int iL = indL[k];
    int iR = indR[k];

    const GCG_Node *srcNode = LeftLegs[iL].first;
    const int srcIndex = LeftLegs[iL].second;
    const GCG_Node *tgtNode = RightLegs[iR].first;
    const int tgtIndex = RightLegs[iR].second;

    std::stringstream ssID, ssLabel;
    ssID << std::hex << e << ":" << std::dec << k;
    ssLabel << "["    << srcNode->getID().getLabel() << ":" << srcIndex
            << " -- " << tgtNode->getID().getLabel() << ":" << tgtIndex << "]";

    bool result = curGraph->addEdge(StringID(ssID.str(), ssLabel.str()),
                                    srcNode, srcIndex, tgtNode, tgtIndex);
    assert(result && "internal error: should not fail to add edge");
  }

  for (int k = 0; k < indL.size(); k++)
    LeftLegs.erase(LeftLegs.begin() + indL[k] - k);
  for (int k = 0; k < indR.size(); k++)
    RightLegs.erase(RightLegs.begin() + indR[k] - k);

  curLegs = savedLegs;
  for (int k = 0; k < LeftLegs.size(); k++) curLegs.push_back(LeftLegs[k]);
  for (int k = 0; k < RightLegs.size(); k++) curLegs.push_back(RightLegs[k]);
}

