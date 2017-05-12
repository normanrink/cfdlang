
#include <assert.h>
#include <sstream>
#include <string>
#include <iostream>
#include <list>


#include "CodeGen/ExprTree.h"


#define EMIT_INDENT(indent) { std::cout.width((indent)); \
                              std::cout << std::left << ""; \
                              std::cout.unsetf(std::ios::adjustfield); }


std::map<ExprNode::ExprKind, std::string> ExprNode::ExprLabel = {
  { EK_Add, "Add" },
  { EK_Sub, "Sub" },
  { EK_Mul, "Mul" },
  { EK_ScalarMul, "ScalarMul" },
  { EK_Div, "Div" },
  { EK_ScalarDiv, "ScalarDiv" },
  { EK_Contraction, "Contraction" },
  { EK_Product, "Product" },
  { EK_Stack, "Stack" },
  { EK_Transposition, "Transposition" },
  { EK_Identifier, "Identifier"}
};


ExprNode::ExprNode(ExprKind ek, int numChildren, const ExprDimensions &dims)
  : ExKind(ek), NumChildren(numChildren), Dims(dims) {
    for (int i = 0; i < getNumChildren(); i++)
      Children.push_back(nullptr);
}

void ExprNode::print(unsigned indent) const {
  std::string str = ExprLabel[getExprKind()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";

  for (int i = 0; i < getNumChildren(); i++)
    getChild(i)->print(indent + str.length() + 1);

  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void ExprNode::deepDelete() const {
  for (int i = 0; i < getNumChildren(); i++) {
    getChild(i)->deepDelete();
    delete getChild(i);
  }
}


#define DEF_EXPR_NODE_CLASS_VISIT(Kind)            \
void Kind##Expr::visit(ExprTreeVisitor *v) const { \
  v->visit##Kind##Expr(this);                      \
}

DEF_EXPR_NODE_CLASS_VISIT(Add)
DEF_EXPR_NODE_CLASS_VISIT(Sub)
DEF_EXPR_NODE_CLASS_VISIT(Mul)
DEF_EXPR_NODE_CLASS_VISIT(ScalarMul)
DEF_EXPR_NODE_CLASS_VISIT(Div)
DEF_EXPR_NODE_CLASS_VISIT(ScalarDiv)
DEF_EXPR_NODE_CLASS_VISIT(Product)
DEF_EXPR_NODE_CLASS_VISIT(Contraction)
DEF_EXPR_NODE_CLASS_VISIT(Stack)
DEF_EXPR_NODE_CLASS_VISIT(Transposition)
DEF_EXPR_NODE_CLASS_VISIT(Identifier)

#undef DEF_EXPR_NODE_CLASS_VISIT


#define DEF_EXPR_NODE_CLASS_TRANSFORM(Kind)          \
void Kind##Expr::transform(ExprTreeTransformer *t) { \
  t->transform##Kind##Expr(this);                    \
}

DEF_EXPR_NODE_CLASS_TRANSFORM(Add)
DEF_EXPR_NODE_CLASS_TRANSFORM(Sub)
DEF_EXPR_NODE_CLASS_TRANSFORM(Mul)
DEF_EXPR_NODE_CLASS_TRANSFORM(ScalarMul)
DEF_EXPR_NODE_CLASS_TRANSFORM(Div)
DEF_EXPR_NODE_CLASS_TRANSFORM(ScalarDiv)
DEF_EXPR_NODE_CLASS_TRANSFORM(Product)
DEF_EXPR_NODE_CLASS_TRANSFORM(Contraction)
DEF_EXPR_NODE_CLASS_TRANSFORM(Stack)
DEF_EXPR_NODE_CLASS_TRANSFORM(Transposition)
DEF_EXPR_NODE_CLASS_TRANSFORM(Identifier)

#undef DEF_EXPR_NODE_CLASS_TRANSFORM


ScalarMulExpr::ScalarMulExpr(ExprNode *lhs, ExprNode *rhs)
  : ExprNode(EK_ScalarMul, 2, rhs->getDims()) {
  setChild(0, lhs);
  setChild(1, rhs);
}


ScalarDivExpr::ScalarDivExpr(ExprNode *lhs, ExprNode *rhs)
: ExprNode(EK_ScalarDiv, 2, lhs->getDims()) {
  setChild(0, lhs);
  setChild(1, rhs);
}


ProductExpr::ProductExpr(ExprNode *lhs, ExprNode *rhs)
  : ExprNode(EK_Product, 2) {
  setChild(0, lhs);
  setChild(1, rhs);

  ExprDimensions lhsDims = lhs->getDims();
  const ExprDimensions &rhsDims = rhs->getDims();
  for (int i = 0; i < rhsDims.size(); i++)
    lhsDims.push_back(rhsDims[i]);

  setDims(lhsDims);
}


ContractionExpr::ContractionExpr(ExprNode *lhs,
                                 const CodeGen::List &leftIndices,
                                 ExprNode *rhs,
                                 const CodeGen::List &rightIndices)
  : ExprNode(EK_Contraction, 2),
    LeftIndices(leftIndices),
    RightIndices(rightIndices) {
  setChild(0, lhs);
  setChild(1, rhs);

  ExprDimensions lhsDims = lhs->getDims();
  ExprDimensions rhsDims = rhs->getDims();

  std::list<int> leftIndicesList(leftIndices.begin(), leftIndices.end());
  leftIndicesList.sort();
  std::list<int> rightIndicesList(rightIndices.begin(), rightIndices.end());
  rightIndicesList.sort();
  // the following only works if 'leftIndicesList'
  // and 'rightIndicesList' are sorted:
  int erased = 0;
  for (const int index: leftIndicesList) {
    lhsDims.erase(lhsDims.begin() + index - (erased++));
  }
  erased = 0;
  for (const int index: rightIndicesList) {
    rhsDims.erase(rhsDims.begin() + index - (erased++));
  }

  for (int i = 0; i < rhsDims.size(); i++)
    lhsDims.push_back(rhsDims[i]);

  setDims(lhsDims);
}

void ContractionExpr::print(unsigned indent) const {
  std::string str = ExprLabel[getExprKind()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";

  getChild(0)->print(indent + str.length() + 1);
  EMIT_INDENT(indent + str.length() + 1)
  std::cout << CodeGen::getListString(getLeftIndices()) << "\n";

  getChild(1)->print(indent + str.length() + 1);
  EMIT_INDENT(indent + str.length() + 1)
  std::cout << CodeGen::getListString(getRightIndices()) << "\n";

  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}


StackExpr::StackExpr(const std::vector<ExprNode *> &members)
  : ExprNode(EK_Stack, members.size()) {
  for (int i = 0; i < members.size(); i++)
    setChild(i, members[i]);

  ExprDimensions dims;
  dims.push_back(members.size());
  if (members.size()) {
    // by type checking, all members have the same dimensions; hence, it
    // is OK to use only the 0-th member here:
    const ExprDimensions &memberDims = members[0]->getDims();
    for (int i = 0; i < memberDims.size(); i++)
      dims.push_back(memberDims[i]);
  }

  setDims(dims);
}


TranspositionExpr::TranspositionExpr(ExprNode *en,
                                     const CodeGen::TupleList &indexPairs)
  : ExprNode(EK_Transposition, 1),
    IndexPairs(indexPairs) {
  setChild(0, en);

  ExprDimensions dimsToTranspose = en->getDims();
  for (const auto &p: indexPairs) {
    assert(p.size() == 2);
    const int dim0 = dimsToTranspose[p[0]];
    dimsToTranspose[p[0]] = dimsToTranspose[p[1]];
    dimsToTranspose[p[1]] = dim0;
  }

  setDims(dimsToTranspose);
}

void TranspositionExpr::print(unsigned indent) const {
  std::string str = ExprLabel[getExprKind()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";

  getChild(0)->print(indent + str.length() + 1);
  EMIT_INDENT(indent + str.length() + 1)
  std::cout << CodeGen::getTupleListString(getIndexPairs()) << "\n";

  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}


void IdentifierExpr::print(unsigned indent) const {
  std::string str = ExprLabel[getExprKind()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str()
            << " \"" << getName() << "\")\n";
}

const std::string IdentifierExpr::getIndex(unsigned i) const {
  assert(i < Indices.size()
         && "internal error: index out of bounds for identifier indices");
  return Indices.at(i);
}


ExprNodeBuilder::~ExprNodeBuilder() {
  for (auto *en: AllocatedNodes)
    delete en;
}

#define DEF_BUILDER_CREATE_EXPR_NODE(Kind)                             \
Kind##Expr *ExprNodeBuilder::create##Kind##Expr(ExprNode *lhs,         \
                                                ExprNode *rhs) {       \
  Kind##Expr *result = Kind ##Expr::create(lhs, rhs);                  \
  AllocatedNodes.insert(result);                                       \
  return result;                                                       \
}

DEF_BUILDER_CREATE_EXPR_NODE(Add)
DEF_BUILDER_CREATE_EXPR_NODE(Sub)
DEF_BUILDER_CREATE_EXPR_NODE(Mul)
DEF_BUILDER_CREATE_EXPR_NODE(ScalarMul)
DEF_BUILDER_CREATE_EXPR_NODE(Div)
DEF_BUILDER_CREATE_EXPR_NODE(ScalarDiv)
DEF_BUILDER_CREATE_EXPR_NODE(Product)

#undef DEF_BUILDER_CREATE_EXPR_NODE

ContractionExpr *
ExprNodeBuilder::createContractionExpr(ExprNode *lhs,
                                       const CodeGen::List &leftIndices,
                                       ExprNode *rhs,
                                       const CodeGen::List &rightIndices) {
  ContractionExpr *result = ContractionExpr::create(lhs, leftIndices,
                                                    rhs, rightIndices);
  AllocatedNodes.insert(result);
  return result;
}

StackExpr *
ExprNodeBuilder::createStackExpr(const std::vector<ExprNode *> &members) {
  StackExpr *result = StackExpr::create(members);
  AllocatedNodes.insert(result);
  return result;
}

TranspositionExpr *
ExprNodeBuilder::createTranspositionExpr(ExprNode *en,
                                         const CodeGen::TupleList &indexPairs) {
  TranspositionExpr *result = TranspositionExpr::create(en, indexPairs);
  AllocatedNodes.insert(result);
  return result;

}

IdentifierExpr *
ExprNodeBuilder::createIdentifierExpr(const std::string &name,
                                      const ExprNode::ExprDimensions &dims) {
  IdentifierExpr *result = IdentifierExpr::create(name, dims);
  AllocatedNodes.insert(result);
  return result;
}
