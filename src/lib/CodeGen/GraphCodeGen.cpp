
#include "assert.h"
#include <functional>
#include <vector>

#include "codegen.h"
#include "type.h"


#define TEMP_MAP_ASSERT(expr) {                                  \
  if (ExprTemps.find((expr)) == ExprTemps.end())                 \
    assert(0 && "internal error: no temporary for 'Expr' node"); \
  }


bool CodeGen::allCompare(const List &list, Comparison cmp, int pivot) {
  std::function<bool(int)> compare = [cmp, pivot](int i) {
    switch (cmp) {
    case CMP_Less:         return i <  pivot;
    case CMP_LessEqual:    return i <= pivot;
    case CMP_Equal:        return i == pivot;
    case CMP_GreaterEqual: return i >= pivot;
    case CMP_Greater:      return i >  pivot;
    default:
      assert(0 && "internal error: invalid comparison");
    }
  }; 

  for (const auto &i : list) {
    if (!compare(i))
      return false;
  }
  return true;
}

bool CodeGen::isPairList(const TupleList &list) {
  for (const auto &l : list) {
    if (l.size() != 2)
      return false;
  }
  return true;
}

bool CodeGen::partitionPairList(int pivot, const TupleList &list,
                                TupleList &left, TupleList &right,
                                TupleList &mixed) {
  if (!isPairList(list))
    return false;

  left.clear(); right.clear(); mixed.clear();

  for (const auto &l : list) {
    if (allCompare(l, CMP_Less, pivot)) {
      left.push_back(l);   
    } else if (allCompare(l, CMP_GreaterEqual, pivot)) {
      right.push_back(l);
    } else {
      mixed.push_back(l);
    }
  }

  return true;
}

void CodeGen::shiftList(int shiftAmount, List &list) {
  for (int i = 0; i < list.size(); i++)
    list[i] += shiftAmount;
}

void CodeGen::shiftTupleList(int shiftAmount, TupleList &tuples) {
  for (auto &t: tuples)
    shiftList(shiftAmount, t);
}

void CodeGen::flattenTupleList(const TupleList &list, std::list<int> &result) {
  result.clear();
  for (const auto &tuple: list) {
    for (int elem: tuple)
      result.push_back(elem);
  }
  result.sort();
}

void CodeGen::unpackPairList(const TupleList &list, List &left, List &right) {
  assert(isPairList(list));

  left.clear(); right.clear();
  for (const auto &tuple: list) {
    int l = (tuple[0] < tuple[1]) ? tuple[0] : tuple[1];
    int r = (tuple[0] < tuple[1]) ? tuple[1] : tuple[0];

    left.push_back(l); right.push_back(r);
  }
}

void CodeGen::adjustForContractions(List &indices,
                                    const TupleList &contractions,
                                    bool up) {
  assert(isPairList(contractions));

  // FIXME: The following nested loop has a runtime that
  // is roughly quadratic in the size of 'contractions'
  // (assuming 'indices.size()' ~ 'contractions.size()').
  if (!up) {
    for (int i = 0; i < indices.size() ; i++) {
      int index = indices[i];
      // determine the number of contracted indices
      // that are smaller than 'index':
      int adj = 0;
      for (const Tuple &t: contractions)
        adj += (t[0] < index) + (t[1] < index);

      indices[i] -= adj;
    }
  } else {
    for (int i = 0; i < indices.size() ; i++) {
      int index = indices[i];

      int adj = 0;
      for (const Tuple &t: contractions)
        adj += (t[0] <= index) + (t[1] <= index);

      indices[i] += adj;
    }
  }
}


void NumpyCodeGen::visitProgram(const Program *p) {
  append("import numpy as " + getPrefix() + "\n\n");

  ASTVisitor::visitProgram(p);
}

void NumpyCodeGen::visitDecl(const Decl *d) {
  if (d->getNodeType() == NT_TypeDecl)
    return;

  const std::string &name = d->getIdentifier()->getName();
  const Symbol *sym = TheSema->getSymbol(name);
  append(sym->getName() + " = ");
  
  const TensorType &type = sym->getType();
  const std::string init = getPrefix() + "random.rand("
                           + type.getDimString() + ")\n";
  append(init);
}

void NumpyCodeGen::visitStmt(const Stmt *s) {
  const std::string &name = s->getIdentifier()->getName();
  const Symbol *sym = TheSema->getSymbol(name);
  
  const Expr *expr = s->getExpr();
  expr->visit(this);
  TEMP_MAP_ASSERT(expr)

  const std::string &temp = getTempForExpr(expr);
  append(sym->getName() + " = " + temp + "\n");
}

static const std::string getTupleListString(const CodeGen::TupleList &list) {
  std::string result = "[";
  for (int l = 0; l < list.size(); l++) {
    result += "[";
    auto &tuple = list[l];
    for (int i = 0; i < tuple.size(); i++) {
      result += std::to_string(tuple[i]);
      if (i != (tuple.size()-1)) result += ", ";
    }
    result += "]";
    if (l != (list.size()-1)) result += ", ";
  }
  result += "]";
  return result;
}

static const BinaryExpr *extractTensorExprOrNull(const Expr *e) {
  const BinaryExpr *tensor = dynamic_cast<const BinaryExpr *>(e);
  if (!tensor) {
    const ParenExpr *paren = dynamic_cast<const ParenExpr *>(e);
    if (paren)
      return extractTensorExprOrNull(paren->getExpr());
    else
      return nullptr;
  }
  // this should not be 'nullptr' if we get here:
  assert(tensor);

  if (tensor->getNodeType() != NT_TensorExpr)
    return nullptr;

  return tensor;
}

const std::string NumpyCodeGen::getTensorDotString(const std::string &r,
                                                   const std::string &t0,
                                                   const std::string &t1,
                                                   const std::string axes) {
  const std::string &prefix = r + " = " + getPrefix()
                              + ".tensordot(" + t0 + ", " + t1 + ", ";
  if (!axes.length())
    return prefix + "axes=0)\n";
  else
    return prefix + "axes=" + axes + ")\n";
}

const std::string NumpyCodeGen::visitContraction(const Expr *e,
                                                 const TupleList &indices) {
  if (indices.empty()) {
    e->visit(this);
    TEMP_MAP_ASSERT(e);
    return getTempForExpr(e);
  }
  
  const BinaryExpr *tensor = extractTensorExprOrNull(e);
  if (!tensor)
    assert(0 && "internal error: cannot handle general contractions yet");

  if (!isPairList(indices))
    assert(0 && "internal error: only pairs of indices can be contracted");

  const Expr *tensorL = tensor->getLeft();
  const Expr *tensorR = tensor->getRight();
  const TensorType *typeL = TheSema->getType(tensorL);
  int rankL = typeL->getRank();

  TupleList contrL, contrR, contrMixed;
  // classify index pairs into the following categories:
  // - belonging to contractions of the left sub-expression ('contrL')
  // - belonging to contractions of the right sub-expression ('contrR')
  // - having one index from each sub-expression ('contrMixed')
  partitionPairList(rankL, indices, contrL, contrR, contrMixed);

  const std::string tempL = visitContraction(tensorL, contrL);

  // determine the rank of the resulting left sub-expression after
  // contraction has been performed over the set of index pairs 'contrL':
  int rankContractedL = rankL - 2*contrL.size();

  // the index pairs of the right sub-expression must be adjusted by
  // the rank of the left sub-expression:
  TupleList shiftedR = contrR;
  shiftTupleList(-rankL, shiftedR);
  const std::string tempR = visitContraction(tensorR, shiftedR);

  const std::string result = getTemp();

  if (contrMixed.empty()) {
    append(getTensorDotString(result, tempL, tempR));
    return result;
  }

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

  TupleList indicesLR;
  indicesLR.push_back(indL);
  indicesLR.push_back(indR);
  append(getTensorDotString(result, tempL, tempR,
                            getTupleListString(indicesLR)));
  return result;
}

void NumpyCodeGen::visitBinaryExpr(const BinaryExpr *be) {
  switch (be->getNodeType()) {
  case NT_TensorExpr: {
    const Expr *left = be->getLeft();
    left->visit(this);
    TEMP_MAP_ASSERT(left);
    const std::string &t0 = getTempForExpr(left);

    const Expr *right = be->getRight();
    right->visit(this);
    TEMP_MAP_ASSERT(right);
    const std::string &t1 = getTempForExpr(right);

    const std::string &temp = addTempForExpr(be);
    append(getTensorDotString(temp, t0, t1));
    return;
  }
  case NT_DotExpr: {
    const BinaryExpr *tensor = extractTensorExprOrNull(be->getLeft());
    if (!tensor)
      assert(0 && "internal error: cannot handle general contractions yet");

    TupleList contractionsList;
    if (!TheSema->isListOfLists(be->getRight(), contractionsList))
      assert(0 && "internal error: cannot have a non-list here");

    if (contractionsList.empty())
      assert(0 && "internal error: cannot have an empty list here");

    const std::string temp = visitContraction(tensor, contractionsList);
    addNameForExpr(be, temp);
    return;
  }
  default:
    assert(0 && "internal error: invalid binary expression");
  }
}

void NumpyCodeGen::visitIdentifier(const Identifier *id) {
  addNameForExpr(id, id->getName());
}

void NumpyCodeGen::visitInteger(const Integer *i) {
  const std::string &temp = addTempForExpr(i);
  append(temp + " = " + std::to_string(i->getValue()));
}

void NumpyCodeGen::visitBrackExpr(const BrackExpr *be) {
  const ExprList &exprs = *be->getExprs();
  assert(exprs.size() &&
         "internal error: tensor stack should not be empty here");

  std::string result = addTempForExpr(be);
  result += " = " + getPrefix() + ".stack([";
  for (unsigned i = 0; i < exprs.size(); i++) {
    const Expr *e = exprs[i];
    if (i > 0)  result += ", ";
    e->visit(this);
    TEMP_MAP_ASSERT(e);
    result += getTempForExpr(e);
  }
  result +=("], axis=0)\n");
  append(result);
} 

void NumpyCodeGen::visitParenExpr(const ParenExpr *pe) {
  const Expr *e = pe->getExpr();
  e->visit(this);
  TEMP_MAP_ASSERT(e);
  addNameForExpr(pe, getTempForExpr(e));
}


void TheanoCodeGen::constructTypes() {
  const Sema &sema = *getSema();

  for (auto i = sema.types_begin(), e = sema.types_end();
       i != e; i++) {
    const TensorType *type = *i;
    const std::string temp = getTemp();
    append(temp + " = " + getPrefix()
           + ".TensorType('float64', (False,)*"
           + std::to_string(type->getRank()) + ")\n");
    TypeTemps[type] = temp;
  }
}

void TheanoCodeGen::visitProgram(const Program *p) {
  append("from theano import *\n");
  append("import theano.tensor as " + getPrefix() + "\n\n");

  constructTypes();
  append("\n"); 

  ASTVisitor::visitProgram(p); 
  append("\n");

  const Sema &sema = *getSema();
  if (sema.inputs_size() == 0 || sema.outputs_size() == 0)
    return;

  #define IO_SYMBOL_LIST(inout)              \
    std::string inout##List;                 \
    {                                        \
      inout##List = "[";                     \
      bool first = true;                     \
      for (auto i = sema.inout##_begin(),    \
                e = sema.inout##_end();      \
           i != e; i++) {                    \
        const Symbol *sym = *i;              \
        if (!first) inout##List += ", ";     \
        inout##List += sym->getName();       \
        first = false;                       \
      }                                      \
      inout##List += "]";                    \
    }
  
  IO_SYMBOL_LIST(inputs)

  std::string output;
  if (sema.outputs_size() == 1) {
    const Symbol *sym = *sema.outputs_begin();
    append("f = function(" + inputsList + ", " + sym->getName() + ")\n");
  } else {
    IO_SYMBOL_LIST(outputs)
    append("f = function(" + inputsList + ", " + outputsList + ")\n");
  }
}

void TheanoCodeGen::visitDecl(const Decl *d) {
  // FIXME: perhaps the generated Python codes should
  // reflect the user-defined type names
  if (d->getNodeType() == NT_TypeDecl)
   return;

  const std::string &name = d->getIdentifier()->getName();
  const Symbol *sym = getSema()->getSymbol(name);
  const TensorType *type = &sym->getType();
  const std::string &temp = TypeTemps[type];

  append(sym->getName() + " = " + getPrefix()
         + ".TensorVariable(" + temp + ")\n");
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

GraphCodeGen::~GraphCodeGen() {
  for (const auto &it : Graphs) delete it.second;
}

void GraphCodeGen::visitProgram(const Program *p) {
  ASTVisitor::visitProgram(p);
  
  for(const auto &g : Graphs) g.second->plot();
}

void GraphCodeGen::visitStmt(const Stmt *s) {
  const std::string &name = s->getIdentifier()->getName();

  cur = new TensorGraph;
  const Expr *expr = s->getExpr();
  expr->visit(this);

  Graphs[name] = cur;
}

void GraphCodeGen::visitBrackExpr(const BrackExpr *be) {
  assert("internal error: tensor stacks not implemented yet");
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

  visitContraction(tensorL, contrL);

  // determine the rank of the resulting left sub-expression after
  // contraction has been performed over the set of index pairs 'contrL':
  int rankContractedL = rankL - 2*contrL.size();

  // the index pairs of the right sub-expression must be adjusted by
  // the rank of the left sub-expression:
  TupleList shiftedR = contrR;
  shiftTupleList(-rankL, shiftedR);
  visitContraction(tensorR, shiftedR);

  if (contrMixed.empty())
    return;

  List indL, indR;
  unpackPairList(contrMixed, indL, indR);
  
  // only contractions in 'contrL' affect the adjustments
  // of the left indices in 'indL':
  //adjustForContractions(indL, contrL);
  // adjustments of the right indices in 'indR' are affected by
  // the contractions in both 'contrL' and 'contrR':
  //adjustForContractions(indR, contrL); adjustForContractions(indR, contrR);
  
  // the indices to be contracted over in the right sub-expression
  // must be relative to the first index of the right sub-expresion:
  shiftList(-rankContractedL, indR);

  assert(indL.size() == indR.size() &&
         "internal error: mis-matched numbers of indices to be contracted");

  for (int k = 0; k < indL.size(); k++) {
    int iL = indL[k];
    NameExtractor LeftNameExtr(getSema(), iL);
    tensorL->visit(&LeftNameExtr);

    int iR = indR[k];
    NameExtractor RightNameExtr(getSema(), iR);
    tensorR->visit(&RightNameExtr);

    /*GraphNode *leftNode = cur->getNode(LeftNameExtr.getId(), 
                                       LeftNameExtr.getLabel(),
                                       LeftNameExtr.getRank());
    GraphNode *rightNode = cur->getNode(RightNameExtr.getId(),
                                        RightNameExtr.getLabel(),
                                        RightNameExtr.getRank());
*/
    cur->addEdge(GraphEdge::NdIdxPair(LeftNameExtr.getId(),
                                      LeftNameExtr.getIndex()),
                 GraphEdge::NdIdxPair(RightNameExtr.getId(),
                                      RightNameExtr.getIndex()));  
  }
}

void GraphCodeGen::visitParenExpr(const ParenExpr *pe) {
  const Expr *e = pe->getExpr();
  e->visit(this);
}

void GraphCodeGen::visitIdentifier(const Identifier *id) {
  if (cur->existsNode(id))
    return;

  cur->addNode(id, id->getName(), getSema()->getType(id)->getRank());
}

void GraphCodeGen::visitInteger(const Integer *i) {
  assert(0 &&
         "internal error: integer should not be visited for graph generation");
}

void NameExtractor::visitBinaryExpr(const BinaryExpr *be) {
  switch (be->getNodeType()) {
  case NT_TensorExpr: {
    const Expr *left = be->getLeft();
    int leftRank = TheSema->getType(left)->getRank();

    const Expr *right = be->getRight();
    int rightRank = TheSema->getType(right)->getRank();

    if (inheritedIndex < leftRank) {
      left->visit(this);
    } else {
      inheritedIndex -= leftRank;
      right->visit(this);
    }
    return;
  }
  case NT_DotExpr: {
    const BinaryExpr *tensor = extractTensorExprOrNull(be->getLeft());
    if (!tensor)
      assert(0 && "internal error: cannot handle general contractions yet");

    TupleList contractionsList;
    if (!TheSema->isListOfLists(be->getRight(), contractionsList))
      assert(0 && "internal error: cannot have a non-list here");

    if (contractionsList.empty())
      assert(0 && "internal error: cannot have an empty list here");

    if (!isPairList(contractionsList))
      assert(0 && "internal error: only pairs of indices can be contracted");

    const Expr *tensorL = tensor->getLeft();
    const Expr *tensorR = tensor->getRight();
    const TensorType *typeL = TheSema->getType(tensorL);
    int rankL = typeL->getRank();

    TupleList contrL, contrR, contrMixed;
    // classify index pairs into the following categories:
    // - belonging to contractions of the left sub-expression ('contrL')
    // - belonging to contractions of the right sub-expression ('contrR')
    // - having one index from each sub-expression ('contrMixed')
    partitionPairList(rankL, contractionsList, contrL, contrR, contrMixed);

    // determine the rank of the resulting left sub-expression after
    // contraction has been performed over the set of index pairs 'contrL':
    int rankContractedL = rankL - 2*contrL.size();

    List l; l.push_back(inheritedIndex);
    adjustForContractions(l, contrL, true);
    adjustForContractions(l, contrMixed, true);
    adjustForContractions(l, contrR, true);

    if (l[0] < rankL) {
      inheritedIndex = l[0];
      tensorL->visit(this);
    } else {
      inheritedIndex = l[0] - rankL;
      tensorR->visit(this);
    }
    return;
  }
  default:
    assert(0 && "internal error: invalid binary expression");
  }
}

void NameExtractor::visitIdentifier(const Identifier *id) {
  const TensorType *type = TheSema->getType(id);
  assert(inheritedIndex < type->getRank() && "internal error: index invalid");

  idResult = id;
  indexResult = inheritedIndex;
  rankResult  = type->getRank();
}

void NameExtractor::visitInteger(const Integer *i) {
  assert("internal error: no name can be extracted from an integer");
}

void NameExtractor::visitBrackExpr(const BrackExpr *be) {
  assert("internal error: tensor stacks not supported yet");
}

void NameExtractor::visitParenExpr(const ParenExpr *pe) {
  pe->getExpr()->visit(this);  
}

BottomUpGraphCodeGen::~BottomUpGraphCodeGen() {
  for (const auto &it : Graphs) delete it.second;
}

void BottomUpGraphCodeGen::visitProgram(const Program *p) {
  ASTVisitor::visitProgram(p);
  
  for(const auto &g : Graphs) g.second->plot();
}

void BottomUpGraphCodeGen::visitStmt(const Stmt *s) {
  const std::string &name = s->getIdentifier()->getName();

  cur = new TensorGraph;
  const Expr *expr = s->getExpr();
  expr->visit(this);

  Graphs.push_back(std::pair<const std::string, TensorGraph*>(name, cur));
}

void BottomUpGraphCodeGen::visitIdentifier(const Identifier *id) {
  int rank = getSema()->getType(id)->getRank();
  cur->addNode(id, id->getName(), rank);

  for (int i = 0; i < rank; i++)
    Legs.push_back(GraphEdge::NdIdxPair(id, i));
}

void BottomUpGraphCodeGen::visitInteger(const Integer *i) {
  assert(0 &&
         "internal error: integer should not be visited for graph generation");
}

void BottomUpGraphCodeGen::visitParenExpr(const ParenExpr *pe) {
  const Expr *e = pe->getExpr();
  e->visit(this);
}

void BottomUpGraphCodeGen::visitBrackExpr(const BrackExpr *be) {
  const ExprList &exprs = *be->getExprs();

  TensorGraph *current = cur;

  std::string stack = "[";
  for (unsigned i = 0; i < exprs.size(); i++) {
    cur = new TensorGraph();
    exprs[i]->visit(this);

    std::string t = getTemp();
    Graphs.push_back(std::pair<const std::string, TensorGraph*>(t, cur));
    
    if (i > 0)  stack += ", ";
    stack += t;
  }
  stack += "]";

  cur = current;
  int rank = getSema()->getType(be)->getRank();
  cur->addNode(be, stack, rank);

  Legs.clear();
  for (int i = 0; i < rank; i++)
    Legs.push_back(GraphEdge::NdIdxPair(be, i));
} 

void BottomUpGraphCodeGen::visitBinaryExpr(const BinaryExpr *be) {
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

void BottomUpGraphCodeGen::visitContraction(const Expr *e, const TupleList &indices) {
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

  Legs.clear();
  visitContraction(tensorL, contrL);
  std::vector<GraphEdge::NdIdxPair> LeftLegs = Legs;

  // determine the rank of the resulting left sub-expression after
  // contraction has been performed over the set of index pairs 'contrL':
  int rankContractedL = rankL - 2*contrL.size();

  // the index pairs of the right sub-expression must be adjusted by
  // the rank of the left sub-expression:
  TupleList shiftedR = contrR;
  shiftTupleList(-rankContractedL, shiftedR);

  Legs.clear();
  visitContraction(tensorR, shiftedR);
  std::vector<GraphEdge::NdIdxPair> RightLegs = Legs;

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

    cur->addEdge(LeftLegs[iL], RightLegs[iR]);
  }

  for (int k = 0; k < indL.size(); k++)
    LeftLegs.erase(LeftLegs.begin() + indL[k] - k);
  for (int k = 0; k < indR.size(); k++)
    RightLegs.erase(RightLegs.begin() + indR[k] - k);

  Legs.clear();
  for (int k = 0; k < LeftLegs.size(); k++) Legs.push_back(LeftLegs[k]);
  for (int k = 0; k < RightLegs.size(); k++) Legs.push_back(RightLegs[k]);
}

