
#include <assert.h>
#include <functional>
#include <list>
#include <vector>


#include "CodeGen/ExprTree.h"
#include "CodeGen/CodeGen.h"
#include "Sema/TensorType.h"


CodeGen::CodeGen(const Sema *sema, const std::string &functionName)
  : TheSema(sema), TempCounter(0), Code(""), FunctionName(functionName) {
  ENBuilder = new ExprNodeBuilder;
}

CodeGen::~CodeGen() {
  delete ENBuilder;
}

std::string CodeGen::getTemp() {
  return "t" + std::to_string(TempCounter++);
}

void CodeGen::addFunctionArgument(const std::string &name) {
  const int position = FunctionArguments.size();
  FunctionArguments.push_back({position, name});
}

void CodeGen::EXPR_TREE_MAP_ASSERT(const Expr *expr) const {
  if (ExprTrees.find((expr)) == ExprTrees.end())
    assert(0 && "internal error: no expression tree for 'Expr' node");
}

void CodeGen::visitDecl(const Decl *d) {
  Declarations.push_back(d);
}

void CodeGen::visitStmt(const Stmt *s) {
  Statements.push_back(s);
}

void CodeGen::addAssignment(const Stmt *s) {
  const Sema *sema = getSema();
  const std::string &name = s->getIdentifier()->getName();
  const TensorType &type = sema->getSymbol(name)->getType();
  ExprNode *lhs = ENBuilder->createIdentifierExpr(name, type.getDims());

  const Expr *e = s->getExpr();
  EXPR_TREE_MAP_ASSERT(e);
  ExprNode *rhs = ExprTrees[e];

  Assignments.push_back({lhs, rhs});
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
                                    const TupleList &contractions) {
  assert(isPairList(contractions));

  // FIXME: The following nested loop has a runtime that
  // is roughly quadratic in the size of 'contractions'
  // (assuming 'indices.size()' ~ 'contractions.size()').
  for (int i = 0; i < indices.size() ; i++) {
    int index = indices[i];
    // determine the number of contracted indices
    // that are smaller than 'index':
    int adj = 0;
    for (const Tuple &t: contractions)
      adj += (t[0] < index) + (t[1] < index);

    indices[i] -= adj;
  }
}

const std::string CodeGen::getListString(const List &list) {
  std::string result = "[";
  for (int l = 0; l < list.size(); l++) {
    result += std::to_string(list[l]);
    if (l != (list.size()-1)) result += ", ";
  }
  result += "]";
  return result;
}

const std::string CodeGen::getTupleListString(const TupleList &list) {
  std::string result = "[";
  for (int l = 0; l < list.size(); l++) {
    result += getListString(list[l]);
    if (l != (list.size()-1)) result += ", ";
  }
  result += "]";
  return result;
}


const BinaryExpr *CodeGen::extractTensorExprOrNull(const Expr *e) {
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

  if (tensor->getNodeType() != ASTNode::NT_ProductExpr)
    return nullptr;

  return tensor;
}

